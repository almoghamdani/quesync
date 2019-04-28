#include "communicator.h"

#include <ctime>
#include <chrono>

#include "../../../shared/quesync_exception.h"
#include "../../../shared/utils.h"

#include "../../../shared/packets/ping_packet.h"

Communicator::Communicator() : _socket(nullptr), _connected(false)
{
}

void Communicator::connect(std::string server_ip)
{
    ResponsePacket *response_packet;

    QuesyncError error = SUCCESS;
    tcp::endpoint server_endpoint;
    PingPacket ping_packet;

    // Get the endpoint of the server to connect to
    SocketManager::GetEndpoint(server_ip.c_str(), SERVER_PORT, server_endpoint);

    // If we currently have a socket with the server, close it and delete it
    if (_socket)
    {
        _socket->close();
        delete _socket;
    }

    try
    {
        // Allocate a new socket
        _socket = new tcp::socket(SocketManager::io_context);

        // Try to connect to the server
        _socket->connect(server_endpoint);
    }
    catch (...)
    {
        // Ignore errors here because they will be caught in the ping packet that is being sent to the server
    }

    // Copy the ping packet to the dat buffer
    Utils::CopyString(ping_packet.encode(), _data);

    // Send to the server the ping packet expecting a response
    error = SocketManager::SendServerWithResponse(*_socket, _data, MAX_DATA_LEN);
    if (error)
    {
        throw QuesyncException(error);
    }

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    // If the response is not a pong packet, return unknown error
    if (!error && ((response_packet && response_packet->type() != PONG_PACKET) || !response_packet))
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // Set the socket as connected
    _connected = true;

    // Start the keep alive thread
    _keep_alive_thread = std::thread(&Communicator::keep_alive, this);
    _keep_alive_thread.detach();
}

void Communicator::keep_alive()
{
    clock_t send_clock, recv_clock;

    QuesyncError error = SUCCESS;
    ResponsePacket *response_packet;
    PingPacket ping_packet;

    uint8_t ping_retries = 0;

    while (true)
    {
        // Sleep for a second and a half
        std::this_thread::sleep_for(std::chrono::milliseconds(750));

        // If the socket isn't connected, skip the iteration
        if (!_socket || !_connected)
        {
            continue;
        }

        // Lock the mutex lock
        _socket_lock.lock();

        // Get the current system clock
        send_clock = std::clock();

        // Copy the ping packet to the dat buffer
        Utils::CopyString(ping_packet.encode(), _data);

        // Send to the server the ping packet expecting a response
        error = SocketManager::SendServerWithResponse(*_socket, _data, MAX_DATA_LEN);
        if (error)
        {
            ping_retries++;
            if (ping_retries >= MAX_PING_RETRIES) // If reached the max ping retries, set the socket as not connected
            {
                _connected = false;
                ping_retries = 0;
            }

            std::cout << "Unable to get response from server.." << std::endl;
            continue;
        }

        // Get the system clock after the recv of the pong packet
        recv_clock = std::clock();

        // Unlock socket mutex for other threads trying to access the socket
        _socket_lock.unlock();

        // Parse the response packet
        response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

        // If the response is not a pong packet, return unknown error
        if (!error && ((response_packet && response_packet->type() != PONG_PACKET) || !response_packet))
        {
            std::cout << "Unable to get response from server.." << std::endl;
            continue;
        }

        std::cout << "Ping is " << ms_diff(recv_clock, send_clock) << "ms" << std::endl;
    }
}

ResponsePacket *Communicator::send(SerializedPacket *packet)
{
    QuesyncError error = SUCCESS;
    ResponsePacket *response_packet;

    // If the socket isn't connected, throw error
    if (!_socket || !_connected)
    {
        throw QuesyncError(UNKNOWN_ERROR);
    }

    // Lock the mutex lock
    _socket_lock.lock();

    // Copy the ping packet to the dat buffer
    Utils::CopyString(packet->encode(), _data);

    // Send to the server the ping packet expecting a response
    error = SocketManager::SendServerWithResponse(*_socket, _data, MAX_DATA_LEN);
    if (error)
    {
        throw QuesyncError(error);
    }

    // Unlock socket mutex for other threads trying to access the socket
    _socket_lock.unlock();

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    return response_packet;
}

double Communicator::ms_diff(clock_t end_clock, clock_t start_clock)
{
    double diffticks = end_clock - start_clock;
    double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);

    return diffms;
}