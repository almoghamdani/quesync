#pragma once

#ifdef _WIN32
#include <malloc.h> // Fix _alloca issue with asio
#endif

#include <asio.hpp>

#include <iostream>
#include <thread>

#include "../../../shared/errors.h"

using asio::ip::tcp;
using asio::ip::udp;

class SocketManager
{
  public:
    static asio::io_context io_context;

    template <typename T>
    static void GetEndpoint(const char *ip_address, int port, T &endpoint);

    static QuesyncError SendServerWithResponse(tcp::socket &socket, char *data, const int MAX_RESPONSE_LEN);
    static QuesyncError GetResponse(tcp::socket &socket, char *data, const int MAX_RESPONSE_LEN);

  private:
    static QuesyncError ErrorForSystemError(std::system_error& ex);
};
