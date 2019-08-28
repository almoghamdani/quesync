#pragma once

#ifdef _WIN32
#include <malloc.h>  // Fix _alloca issue with asio
#endif

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <iostream>
#include <thread>

#include "../../../../shared/error.h"

using asio::ip::tcp;
using asio::ip::udp;

namespace quesync {
namespace client {
class socket_manager {
   public:
    static asio::io_context io_context;

    template <typename T>
    static void get_endpoint(const char *ip_address, int port, T &endpoint);

    static void send(asio::ssl::stream<tcp::socket> &socket, const char *data, size_t size);
    static void recv(asio::ssl::stream<tcp::socket> &socket, char *data, const int len);

    static error error_for_system_error(std::system_error &ex);
};

};  // namespace client
};  // namespace quesync
