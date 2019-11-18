#include "communicator.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>

#include "client.h"

#include "../../../../shared/events/ping_event.h"
#include "../../../../shared/exception.h"
#include "../../../../shared/packets/error_packet.h"
#include "../../../../shared/packets/ping_packet.h"
#include "../../../../shared/utils/parser.h"

quesync::client::modules::communicator::communicator(std::shared_ptr<client> client)
    : module(client), _socket(nullptr), _stop_threads(true) {}

void quesync::client::modules::communicator::clean_connection(bool join_recv_thread) {
    // Signal the threads to stop
    _stop_threads = true;

    // Wake up the threads
    _events_cv.notify_one();
    _response_cv.notify_one();

    // If the socket is connected, close the connection
    if (_socket && _socket->lowest_layer().is_open()) {
        // Close the socket
        _socket->lowest_layer().close();
    }

    // If the events thread is still alive, join it
    if (_events_thread.joinable()) {
        try {
            _events_thread.join();
        } catch (...) {
        }
    }

    // If the r receiver is still alive, join it
    if (_receiver_thread.joinable() && join_recv_thread) {
        try {
            _receiver_thread.join();
        } catch (...) {
        }
    }

    // If the keep alive thread is still alive, join it
    if (_keep_alive_thread.joinable()) {
        try {
            _keep_alive_thread.join();
        } catch (...) {
        }
    }

    // If the socket object isn't null
    if (_socket) {
        // Free the socket
        delete _socket;

        // Reset socket ptr
        _socket = nullptr;
    }

    // Reset IP
    _server_ip = "";
}

void quesync::client::modules::communicator::connect(std::string server_ip) {
    std::shared_ptr<response_packet> response_packet;

    tcp::endpoint server_endpoint;

    packets::ping_packet ping_packet;
    std::string res;

    // Check if already connected to the wanted server
    if (_socket && _server_ip == server_ip) {
        return;
    }

    // Get the endpoint of the server to connect to
    socket_manager::get_endpoint(server_ip.c_str(), SERVER_PORT, server_endpoint);

    // If we currently have a socket with the server, clean the connection
    if (_socket) {
        clean_connection();
    }

    try {
        // Allocate a new socket
        _socket = new asio::ssl::stream<tcp::socket>(socket_manager::io_context,
                                                     socket_manager::ssl_context);
        _socket->set_verify_mode(asio::ssl::verify_peer);

        // Try to connect to the server
        _socket->lowest_layer().connect(server_endpoint);

        // Try to handshake
        _socket->handshake(asio::ssl::stream_base::client);
    } catch (std::system_error &ex) {
        clean_connection();

        throw exception(socket_manager::error_for_system_error(ex));
    }

    try {
        // Send to the server the ping packet
        socket_manager::send(*_socket, ping_packet.encode());
    } catch (exception &ex) {
        clean_connection();

        throw exception(ex);
    }

    try {
        // Get from the server the response
        res = socket_manager::recv(*_socket);
    } catch (exception &ex) {
        clean_connection();

        throw exception(ex);
    }

    // Parse the response packet
    response_packet =
        std::static_pointer_cast<quesync::response_packet>(utils::parser::parse_packet(res));

    // If the response is not a pong packet, return unknown error
    if ((response_packet && response_packet->type() != packet_type::pong_packet) ||
        !response_packet) {
        clean_connection();

        throw exception(error::unknown_error);
    }

    // Save the server IP
    _server_ip = server_ip;

    // Stop signaling threads to exit
    _stop_threads = false;

    // Start the events thread
    if (!_events_thread.joinable()) {
        _events_thread = std::thread(&communicator::events_handler, this);
    }

    // Start the receiver thread
    if (!_receiver_thread.joinable()) {
        _receiver_thread = std::thread(&communicator::recv, this);
    }

    // Start the keep alive thread
    if (!_keep_alive_thread.joinable()) {
        _keep_alive_thread = std::thread(&communicator::keep_alive, this);
    }
}

std::string quesync::client::modules::communicator::server_ip() {
    if (_server_ip.empty()) {
        throw exception(error::no_connection);
    }

    return _server_ip;
}

void quesync::client::modules::communicator::recv() {
    while (true) {
        std::shared_ptr<response_packet> response_packet;

        std::string buf;

        // If the socket isn't connected or all threads to be exited, quit the thread
        if (_stop_threads || !_socket || !_socket->lowest_layer().is_open()) {
            break;
        }

        try {
            // Get a response from the server
            buf = socket_manager::recv(*_socket);
        } catch (...) {
            // Clean the connection if the server is disconnected
            clean_connection(false);

            // Call the clean callback
            _client->clean_connection();

            break;
        }

        // Parse the response packet
        response_packet =
            std::static_pointer_cast<quesync::response_packet>(utils::parser::parse_packet(buf));

        // If the resposne is an event, push it to the vector of events
        if (response_packet->type() == packet_type::event_packet) {
            std::unique_lock<std::mutex> lk(_events_mutex);

            // Push the packet as an EventPacket to the vector of events packets
            _event_packets.push_back(
                std::static_pointer_cast<packets::event_packet>(response_packet));

            // Notify the events handler thread that there is new events waiting to be handled
            _events_cv.notify_one();
        } else {
            std::unique_lock<std::mutex> lk(_socket_get_mutex);

            // Push the response packet to the vector
            _response_packets.push_back(response_packet);

            // Notify one of the waiting threads
            _response_cv.notify_one();
        }
    }
}

void quesync::client::modules::communicator::keep_alive() {
    clock_t send_clock, recv_clock;

    packets::ping_packet ping_packet;

    std::string ping_event_name = "ping";

    while (true) {
        std::shared_ptr<response_packet> response_packet;

        // Sleep for a 1.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        std::unique_lock<std::mutex> get_lk(_socket_get_mutex, std::defer_lock);
        std::unique_lock<std::mutex> send_lk(_socket_send_mutex, std::defer_lock);

        // If the socket isn't connected or all threads to be exited, quit the thread
        if (_stop_threads || !_socket || !_socket->lowest_layer().is_open()) {
            break;
        }

        // Lock the socket's locks
        std::lock(get_lk, send_lk);

        // Get the current system clock
        send_clock = std::clock();

        try {
            // Send to the server the ping packet
            socket_manager::send(*_socket, ping_packet.encode());
        } catch (...) {
            continue;
        }

        // Wait for response to enter
        _response_cv.wait(get_lk,
                          [&, this] { return !!_stop_threads || !_response_packets.empty(); });
        if (_stop_threads) {
            break;
        }

        // Get the system clock after the recv of the pong packet
        recv_clock = std::clock();

        // Get the ping packet
        response_packet = _response_packets.back();
        _response_packets.pop_back();

        // If the response is not a pong packet, return unknown error
        if ((response_packet && response_packet->type() != packet_type::pong_packet) ||
            !response_packet) {
            continue;
        }

        // Try to call the ping event
        try {
            _event_handler.call_event(std::static_pointer_cast<event>(
                std::make_shared<events::ping_event>(ms_diff(recv_clock, send_clock))));
        } catch (...) {
            // Ignore errors
        }
    }
}

void quesync::client::modules::communicator::events_handler() {
    while (true) {
        std::unique_lock<std::mutex> events_lk(_events_mutex);

        // While no event packets are waiting to be handled, wait
        _events_cv.wait(events_lk,
                        [&, this] { return !!_stop_threads || !_event_packets.empty(); });
        if (_stop_threads) {
            break;
        }

        // While there are more event packets to handle
        while (!_event_packets.empty()) {
            std::shared_ptr<event> evt;
            std::shared_ptr<packets::event_packet> event_packet;

            // Get the event packet
            event_packet = _event_packets.back();
            _event_packets.pop_back();

            // Get the event from the event packet
            evt = event_packet->event();

            try {
                // Call the event handler
                _event_handler.call_event(evt);
            } catch (std::runtime_error &ex) {
                // Print error
                std::cout << "An error occurred trying to call event: " << ex.what() << std::endl;
            }
        }
    }
}

std::shared_ptr<quesync::response_packet> quesync::client::modules::communicator::send(
    quesync::serialized_packet *packet) {
    std::unique_lock<std::mutex> get_lk(_socket_get_mutex, std::defer_lock);
    std::unique_lock<std::mutex> send_lk(_socket_send_mutex, std::defer_lock);

    std::shared_ptr<response_packet> response_packet;

    // Lock the socket's locks
    std::lock(get_lk, send_lk);

    // If the socket isn't connected, throw error
    if (!_socket) {
        throw error(error::no_connection);
    }

    // Send to the server the packet
    try {
        socket_manager::send(*_socket, packet->encode());
    } catch (std::exception &ex) {
        // Clean the connection
        clean_connection();

        // Call the clean callback
        _client->clean_connection();

        // Re-throw the exception
        throw ex;
    }

    // Wait for response to enter
    while (_response_packets.empty() && !_stop_threads) {
        _response_cv.wait(get_lk);
    }

    // If requested to stop all threads, throw error
    if (_stop_threads) {
        throw exception(error::no_connection);
    }

    // Get the response packet
    response_packet = _response_packets.back();
    _response_packets.pop_back();

    // If the response packet is an error packet, throw
    if (response_packet->type() == packet_type::error_packet) {
        throw exception(std::static_pointer_cast<packets::error_packet>(response_packet)->error());
    }

    return response_packet;
}

std::shared_ptr<quesync::response_packet> quesync::client::modules::communicator::send_and_verify(
    quesync::serialized_packet *packet, quesync::packet_type response_type) {
    std::shared_ptr<response_packet> response_packet;

    // Send the packet and get response
    response_packet = send(packet);
    if (!response_packet || response_packet->type() != response_type) {
        throw exception(error::invalid_packet);
    }

    return response_packet;
}

quesync::client::event_handler &quesync::client::modules::communicator::event_handler() {
    return _event_handler;
}

double quesync::client::modules::communicator::ms_diff(clock_t end_clock, clock_t start_clock) {
    double diffticks = end_clock - start_clock;
    double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);

    return diffms;
}