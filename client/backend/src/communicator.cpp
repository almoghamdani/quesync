#include "communicator.h"

#include <memory>
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
    std::shared_ptr<ResponsePacket> response_packet;

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
    response_packet = std::shared_ptr<ResponsePacket>((ResponsePacket *)Utils::ParsePacket(_data));

    // If the response is not a pong packet, return unknown error
    if (!error && ((response_packet && response_packet->type() != PONG_PACKET) || !response_packet))
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // Set the socket as connected
    _connected = true;

    // Start the receiver thread
    _receiver_thread = std::thread(&Communicator::recv, this);
    _receiver_thread.detach();

    // Start the keep alive thread
    _keep_alive_thread = std::thread(&Communicator::keep_alive, this);
    _keep_alive_thread.detach();
}

void Communicator::recv()
{
    QuesyncError error = SUCCESS;

    while (true)
    {
        std::shared_ptr<ResponsePacket> response_packet;

        // If the socket isn't connected, skip the iteration
        if (!_socket || !_connected)
        {
            continue;
        }

        // Get a response from the server
        error = SocketManager::GetResponse(*_socket, _recv_data, MAX_DATA_LEN);
        if (error)
        {
            // Skip iteration
            continue;
        }

        // Parse the response packet
        response_packet = std::shared_ptr<ResponsePacket>((ResponsePacket *)Utils::ParsePacket(_recv_data));

        // If the resposne is an event, push it to the vector of events
        if (response_packet->type() == EVENT_PACKET)
        {
            _event_packets.push_back(std::static_pointer_cast<EventPacket>(response_packet));
        }
        else
        {
            std::unique_lock<std::mutex> lk(_socket_get_lock);

            // Push the response packet to the vector
            _response_packets.push_back(response_packet);

            // Notify one of the waiting threads
            _response_cv.notify_one();
        }
    }
}

void Communicator::keep_alive()
{
    clock_t send_clock, recv_clock;

    QuesyncError error = SUCCESS;
    PingPacket ping_packet;

    while (true)
    {
        std::shared_ptr<ResponsePacket> response_packet;

        // Sleep for a second and a half
        std::this_thread::sleep_for(std::chrono::milliseconds(750));

        std::unique_lock<std::mutex> get_lk(_socket_get_lock, std::defer_lock);
        std::unique_lock<std::mutex> send_lk(_socket_send_lock, std::defer_lock);

        // If the socket isn't connected, skip the iteration
        if (!_socket || !_connected)
        {
            continue;
        }

        // Lock the socket's locks
        std::lock(get_lk, send_lk);

        // Get the current system clock
        send_clock = std::clock();

        // Copy the ping packet to the dat buffer
        Utils::CopyString(ping_packet.encode(), _data);

        // Send to the server the ping packet
        error = SocketManager::SendServer(*_socket, _data, strlen(_data));
        if (error)
        {
            std::cout << "Unable to send ping packet from server.." << std::endl;
            continue;
        }

        // Wait for response to enter
        while (_response_packets.empty())
        {
            _response_cv.wait(get_lk);
        }

        // Get the system clock after the recv of the pong packet
        recv_clock = std::clock();

        // Get the ping packet
        response_packet = _response_packets.back();
        _response_packets.pop_back();

        // If the response is not a pong packet, return unknown error
        if (!error && ((response_packet && response_packet->type() != PONG_PACKET) || !response_packet))
        {
            std::cout << "Unable to get response from server.." << std::endl;
            continue;
        }

        std::cout << "Ping is " << ms_diff(recv_clock, send_clock) << "ms" << std::endl;
    }
}

std::shared_ptr<ResponsePacket> Communicator::send(SerializedPacket *packet)
{
    std::unique_lock<std::mutex> get_lk(_socket_get_lock, std::defer_lock);
    std::unique_lock<std::mutex> send_lk(_socket_send_lock, std::defer_lock);

    QuesyncError error = SUCCESS;
    std::shared_ptr<ResponsePacket> response_packet;

    // If the socket isn't connected, throw error
    if (!_socket || !_connected)
    {
        throw QuesyncError(UNKNOWN_ERROR);
    }

    // Lock the socket's locks
    std::lock(get_lk, send_lk);

    // Copy the packet to the dat buffer
    Utils::CopyString(packet->encode(), _data);

    // Send to the server the packet
    error = SocketManager::SendServer(*_socket, _data, strlen(_data));
    if (error)
    {
        throw QuesyncError(error);
    }

    // Wait for response to enter
    while (_response_packets.empty())
    {
        _response_cv.wait(get_lk);
    }

    // Get the response packet
    response_packet = _response_packets.back();
    _response_packets.pop_back();

    return response_packet;
}

EventHandler &Communicator::eventHandler()
{
    return _event_handler;
}

double Communicator::ms_diff(clock_t end_clock, clock_t start_clock)
{
    double diffticks = end_clock - start_clock;
    double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);

    return diffms;
}