#pragma once
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "socket-manager.h"

#include "../../../shared/packets/serialized_packet.h"
#include "../../../shared/packets/response_packet.h"
#include "../../../shared/packets/event_packet.h"

#define SERVER_PORT 61110
#define MAX_DATA_LEN 10000

#define MAX_PING_RETRIES 3

class Communicator
{
  public:
    Communicator();

    void connect(std::string server_ip);
    std::shared_ptr<ResponsePacket> send(SerializedPacket *packet);

  private:
    std::vector<std::shared_ptr<EventPacket>> _event_packets;
    std::vector<std::shared_ptr<ResponsePacket>> _response_packets;

    std::mutex _socket_get_lock;
    std::condition_variable _response_cv;

    std::mutex _socket_send_lock;

    bool _connected;
    tcp::socket *_socket;

    char _recv_data[MAX_DATA_LEN];
    char _data[MAX_DATA_LEN];

    std::thread _keep_alive_thread;
    std::thread _receiver_thread;

    void keep_alive();
    void recv();

    // Helper functions
    double ms_diff(clock_t end_clock, clock_t start_clock);
};