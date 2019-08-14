#pragma once
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "socket-manager.h"
#include "event-handler.h"

#include "../../../shared/packets/serialized_packet.h"
#include "../../../shared/packets/response_packet.h"
#include "../../../shared/packets/event_packet.h"

#define SERVER_PORT 61110

#define MAX_PING_RETRIES 3

class Communicator
{
public:
	Communicator(std::function<void()> clean_callback);

	void connect(std::string server_ip);
	std::shared_ptr<ResponsePacket> send(SerializedPacket *packet);
	EventHandler &eventHandler();

private:
	EventHandler _event_handler;

	std::vector<std::shared_ptr<EventPacket>> _event_packets;
	std::vector<std::shared_ptr<ResponsePacket>> _response_packets;

	// Locks
	std::mutex _socket_get_lock;
	std::mutex _socket_send_lock;
	std::mutex _events_lock;

	// Condition Variables
	std::condition_variable _response_cv;
	std::condition_variable _events_cv;

	// Socket
	std::string _server_ip;
	bool _connected;
	unsigned int _ping_retries;
	asio::ssl::stream<tcp::socket> *_socket;
	asio::ssl::context _context;

	// Threads
	std::thread _keep_alive_thread;
	std::thread _receiver_thread;
	std::thread _events_thread;
	void keep_alive();
	void recv();
	void events_handler();

	// Clean Connection
	std::function<void()> _clean_callback;
	void clean_connection();

	// Helper functions
	double ms_diff(clock_t end_clock, clock_t start_clock);

	QuesyncError get_header(Header& header);
	QuesyncError send_with_header(std::string data);
};