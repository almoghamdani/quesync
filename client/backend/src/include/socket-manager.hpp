#pragma once
#include <iostream>
#include <thread>
#include <uv.h>

#include "socket-error.hpp"

class SocketManager
{
public:
    static void InitSocketManager();

    static void CreateUDPSocket(uv_udp_t *socket);
    static void InitReadFunction(uv_udp_t *socket, uv_udp_recv_cb readFunction);

    static void CreateTCPSocket(uv_tcp_t *socket, const char *serverIP, const int port);
    static void Send(uv_tcp_t *socket, void *data, unsigned int nbytes);
};
