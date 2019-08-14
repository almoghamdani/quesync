#include "communicator.h"

#include <iostream>
#include <memory>
#include <ctime>
#include <chrono>

#include "event_names.h"
#include "../../../shared/quesync_exception.h"
#include "../../../shared/header.h"
#include "../../../shared/utils.h"
#include "../../../shared/packets/ping_packet.h"

Communicator::Communicator(std::function<void()> clean_callback)
	: _socket(nullptr),
	  _context(asio::ssl::context::sslv23),
	  _clean_callback(clean_callback)
{
	_context.load_verify_file("cert.pem");
}

void Communicator::clean_connection()
{
	// If we currently have a socket with the server, close it and delete it
	if (_socket)
	{
		// Close the socket
		_socket->lowest_layer().close();

		// Free the socket
		delete _socket;

		// Reset socket ptr
		_socket = nullptr;
	}

	// Reset ping retries
	_ping_retries = 0;

	// Call the clean callback
	_clean_callback();
}

QuesyncError Communicator::send_with_header(std::string data)
{
	Header header = {1, 0};
	std::string full_packet;
	QuesyncError error;

	// Set data size
	header.size = data.size();

	// Combine the header and the packet
	full_packet = Utils::EncodeHeader(header) + data;

	// Send to the server the data
	error = SocketManager::Send(*_socket, full_packet.data(), full_packet.length());

	return error;
}

QuesyncError Communicator::get_header(Header &header)
{
	QuesyncError error;

	char *header_buf = new char[sizeof(Header)];

	// Get from the server the header
	error = SocketManager::Recv(*_socket, header_buf, sizeof(Header));
	if (!error)
	{
		// Decode the header
		header = Utils::DecodeHeader(header_buf);
	}

	return error;
}

void Communicator::connect(std::string server_ip)
{
	std::shared_ptr<ResponsePacket> response_packet;

	QuesyncError error = SUCCESS;
	tcp::endpoint server_endpoint;

	PingPacket ping_packet;

	Header header;

	std::shared_ptr<char> buf;

	// Check if already connected to the wanted server
	if (_socket && _server_ip == server_ip)
	{
		return;
	}

	// Get the endpoint of the server to connect to
	SocketManager::GetEndpoint(server_ip.c_str(), SERVER_PORT, server_endpoint);

	// If we currently have a socket with the server, clean the connection
	if (_socket)
	{
		clean_connection();
	}

	try
	{
		// Allocate a new socket
		_socket = new asio::ssl::stream<tcp::socket>(SocketManager::io_context, _context);
		_socket->set_verify_mode(asio::ssl::verify_peer);

		// Try to connect to the server
		_socket->lowest_layer().connect(server_endpoint);

		// Try to handshake
		_socket->handshake(asio::ssl::stream_base::client);
	}
	catch (...)
	{
		// Ignore errors here because they will be caught in the ping packet that is being sent to the server
	}

	// Send to the server the ping packet
	error = send_with_header(ping_packet.encode());
	if (error)
	{
		clean_connection();

		throw QuesyncException(error);
	}

	// Get from the server the header
	error = get_header(header);
	if (error)
	{
		clean_connection();

		throw QuesyncException(error);
	}

	// Allocate the buffer
	buf = std::shared_ptr<char>(new char[header.size]);

	// Get from the server the response
	error = SocketManager::Recv(*_socket, buf.get(), header.size);
	if (error)
	{
		clean_connection();

		throw QuesyncException(error);
	}

	// Parse the response packet
	response_packet = std::shared_ptr<ResponsePacket>((ResponsePacket *)Utils::ParsePacket(std::string(buf.get(), header.size)));

	// If the response is not a pong packet, return unknown error
	if (!error && ((response_packet && response_packet->type() != PONG_PACKET) || !response_packet))
	{
		clean_connection();

		throw QuesyncException(UNKNOWN_ERROR);
	}

	// Save the server IP
	_server_ip = server_ip;

	// Start the events thread
	if (!_events_thread.joinable())
	{
		_events_thread = std::thread(&Communicator::events_handler, this);
		_events_thread.detach();
	}

	// Start the receiver thread
	if (!_receiver_thread.joinable())
	{
		_receiver_thread = std::thread(&Communicator::recv, this);
		_receiver_thread.detach();
	}

	// Start the keep alive thread
	if (!_keep_alive_thread.joinable())
	{
		_keep_alive_thread = std::thread(&Communicator::keep_alive, this);
		_keep_alive_thread.detach();
	}
}

void Communicator::recv()
{
	QuesyncError error = SUCCESS;

	while (true)
	{
		std::shared_ptr<ResponsePacket> response_packet;

		Header header;
		std::shared_ptr<char> buf;

		// If the socket isn't connected, skip the iteration
		if (!_socket)
		{
			continue;
		}

		// Get the header from the server
		error = get_header(header);
		if (error)
		{
			// Skip iteration
			continue;
		}

		// Allocate the buffer
		buf = std::shared_ptr<char>(new char[header.size]);

		// Get a response from the server
		error = SocketManager::Recv(*_socket, buf.get(), header.size);
		if (error)
		{
			// Skip iteration
			continue;
		}

		// Parse the response packet
		response_packet = std::shared_ptr<ResponsePacket>((ResponsePacket *)Utils::ParsePacket(std::string(buf.get(), header.size)));

		// If the resposne is an event, push it to the vector of events
		if (response_packet->type() == EVENT_PACKET)
		{
			std::unique_lock<std::mutex> lk(_events_lock);

			// Push the packet as an EventPacket to the vector of events packets
			_event_packets.push_back(std::static_pointer_cast<EventPacket>(response_packet));

			// Notify the events handler thread that there is new events waiting to be handled
			_events_cv.notify_one();
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

	std::string ping_event_name = "ping";

	while (true)
	{
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json ping_event_data;

		// Sleep for a second and a half
		std::this_thread::sleep_for(std::chrono::milliseconds(750));

		std::unique_lock<std::mutex> get_lk(_socket_get_lock, std::defer_lock);
		std::unique_lock<std::mutex> send_lk(_socket_send_lock, std::defer_lock);

		// If the socket isn't connected, skip the iteration
		if (!_socket)
		{
			continue;
		}

		// Lock the socket's locks
		std::lock(get_lk, send_lk);

		// Get the current system clock
		send_clock = std::clock();

		// Send to the server the ping packet
		error = send_with_header(ping_packet.encode());
		if (error)
		{
			// Increase the ping retires count and check if reached the max
			_ping_retries += 1;
			if (_ping_retries == MAX_PING_RETRIES)
			{
				// If reached max ping retries, clean the connection
				clean_connection();
			}

			std::cout << "Unable to send ping packet to server.." << std::endl;
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
			// Increase the ping retires count and check if reached the max
			_ping_retries += 1;
			if (_ping_retries == MAX_PING_RETRIES)
			{
				// If reached max ping retries, clean the connection
				clean_connection();
			}

			std::cout << "Unable to get pong response from server.." << std::endl;
			continue;
		}

		// Reset the ping retires
		_ping_retries = 0;

		// Try to call the ping event
		try
		{
			ping_event_data = nlohmann::json{{"ping", (int)ms_diff(recv_clock, send_clock)}};
			_event_handler.callEvent(ping_event_name, ping_event_data);
		}
		catch (...)
		{
			// Ignore errors
		}
	}
}

void Communicator::events_handler()
{
	while (true)
	{
		std::unique_lock<std::mutex> events_lk(_events_lock);

		// While no event packets are waiting to be handled, wait
		while (_event_packets.empty())
		{
			_events_cv.wait(events_lk);
		}

		// While there are more event packets to handle
		while (!_event_packets.empty())
		{
			Event evt;
			std::string event_name;
			std::shared_ptr<EventPacket> event_packet;
			nlohmann::json event_data;

			// Get the event packet
			event_packet = _event_packets.back();
			_event_packets.pop_back();

			// Get the event from the event packet
			evt = event_packet->event();

			// Get the event type from the event names map and the event data
			event_name = EVENT_NAMES.at(evt.event_type());
			event_data = evt.json();

			try
			{
				// Call the event handler
				_event_handler.callEvent(event_name, event_data);
			}
			catch (std::runtime_error &ex)
			{
				// Print error
				std::cout << "An error occurred trying to call event: " << ex.what() << std::endl;
			}
		}
	}
}

std::shared_ptr<ResponsePacket> Communicator::send(SerializedPacket *packet)
{
	std::unique_lock<std::mutex> get_lk(_socket_get_lock, std::defer_lock);
	std::unique_lock<std::mutex> send_lk(_socket_send_lock, std::defer_lock);

	QuesyncError error = SUCCESS;
	std::shared_ptr<ResponsePacket> response_packet;

	// If the socket isn't connected, throw error
	if (!_socket)
	{
		throw QuesyncError(NO_CONNECTION);
	}

	// Lock the socket's locks
	std::lock(get_lk, send_lk);

	// Send to the server the packet
	error = send_with_header(packet->encode());
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