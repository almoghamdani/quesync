#pragma once
#include <iostream>
#include <uv.h>

#include "socket-error.hpp"

class SocketManager
{
public:
    static void InitSocketManager();

    static void CreateUDPSocket(uv_udp_t *socket);
    static void InitReadFunction(uv_udp_t *socket, uv_udp_recv_cb readFunction);
};
