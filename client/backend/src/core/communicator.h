#pragma once
#include "module.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "event_handler.h"
#include "socket_manager.h"

#include "../../../../shared/packets/event_packet.h"
#include "../../../../shared/response_packet.h"
#include "../../../../shared/serialized_packet.h"

#define SERVER_PORT 61110

namespace quesync {
namespace client {
namespace modules {
class communicator : public module {
   public:
    communicator(std::shared_ptr<client> client);

    void connect(std::string server_ip);

    std::string server_ip();

    std::shared_ptr<response_packet> send(serialized_packet *packet);
    std::shared_ptr<response_packet> send_and_verify(serialized_packet *packet,
                                                     packet_type response_type);

    event_handler &event_handler();

    // Clean Connection
    void clean_connection(bool join_recv_thread = true);

   private:
    quesync::client::event_handler _event_handler;

    std::vector<std::shared_ptr<packets::event_packet>> _event_packets;
    std::vector<std::shared_ptr<response_packet>> _response_packets;

    // Locks
    std::mutex _socket_get_mutex;
    std::mutex _socket_send_mutex;
    std::mutex _events_mutex;

    // Condition Variables
    std::condition_variable _response_cv;
    std::condition_variable _events_cv;

    // Socket
    std::string _server_ip;
    bool _connected;
    asio::ssl::stream<tcp::socket> *_socket;

    // Threads
    std::thread _keep_alive_thread;
    std::thread _receiver_thread;
    std::thread _events_thread;
    void keep_alive();
    void recv();
    void events_handler();

    std::atomic<bool> _stop_threads;

    // Helper functions
    double ms_diff(clock_t end_clock, clock_t start_clock);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync
