#pragma once

#ifdef _WIN32
#include <malloc.h> // Fix _alloca issue with asio
#endif

#include <asio.hpp>
#include <asio/ssl.hpp>

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
    
    static QuesyncError Send(asio::ssl::stream<tcp::socket> &socket, const char *data, size_t size);
    static QuesyncError Recv(asio::ssl::stream<tcp::socket> &socket, char *data, const int len);

  private:
    static QuesyncError ErrorForSystemError(std::system_error& ex);
};
