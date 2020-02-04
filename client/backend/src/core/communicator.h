#pragma once
#include "module.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "../../../../shared/packets/event_packet.h"
#include "../../../../shared/response_packet.h"
#include "../../../../shared/serialized_packet.h"
#include "event_handler.h"
#include "socket_manager.h"

#define SERVER_PORT 61110

namespace quesync {
namespace client {
namespace modules {
class communicator : public module {
   public:
    /**
     * Communicator module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    communicator(std::shared_ptr<client> client);

    /**
     * Connect to a server.
     *
     * @param server_ip The server IP to connect to.
     */
    void connect(std::string server_ip);

    /**
     * Gets the server IP that the client is connected to.
     *
     * @return The server IP.
     */
    std::string server_ip();

    /**
     * Sends a packet to the server and get a response from it.
     *
     * @param packet The serialized packet object.
     * @return A shared pointer to a response packet from the server.
     */
    std::shared_ptr<response_packet> send(serialized_packet *packet);

    /**
     * Sends a packet to the server, get a response from it and verify it's type.
     *
     * @param packet The serialized packet object.
     * @param response_type The type of the response.
     * @return A shared pointer to a response packet from the server.
     */
    std::shared_ptr<response_packet> send_and_verify(serialized_packet *packet,
                                                     packet_type response_type);

    /**
     * Gets the event handler object.
     *
     * @return The event handler object.
     */
    event_handler &event_handler();

    /**
     * Cleans the connection with the server.
     *
     * @param join_recv_thread Should the recv thread be joined.
     */
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
