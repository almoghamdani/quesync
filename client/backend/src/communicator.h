#pragma once
#include <thread>
#include <mutex>

#include "socket-manager.h"

#include "../../../shared/packets/serialized_packet.h"
#include "../../../shared/packets/response_packet.h"

#define SERVER_PORT 61110
#define MAX_DATA_LEN 10000

class Communicator
{
  public:
    Communicator();

    void connect(std::string server_ip);
    ResponsePacket *send(SerializedPacket *packet);

  private:
    std::mutex _socket_lock;
    tcp::socket *_socket;
    char _data[MAX_DATA_LEN];

    std::thread _keep_alive_thread;

    void keep_alive();
    double ms_diff(clock_t end_clock, clock_t start_clock);
};