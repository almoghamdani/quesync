#pragma once

#include <malloc.h> // Fix _alloca issue with asio

#include <asio.hpp>

#include <iostream>
#include <thread>

#include "socket-error.hpp"

using asio::ip::udp;
using asio::ip::tcp;

class SocketManager
{
public:
    static asio::io_context io_context;

    template <typename T>
    static void GetEndpoint(const char *ipAddress, int port, T& endpoint);

};
