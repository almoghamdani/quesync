#pragma once

#ifdef _WIN32
#include <malloc.h>  // Fix _alloca issue with asio
#endif

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <iostream>
#include <thread>
#include <string>

#include "../../../../shared/error.h"
#include "../../../../shared/header.h"

using asio::ip::tcp;
using asio::ip::udp;

namespace quesync {
namespace client {
class socket_manager {
   public:
    static asio::io_context io_context;
    static asio::ssl::context ssl_context;

    static void init();

    template <typename T>
    static void get_endpoint(const char *ip_address, int port, T &endpoint);

    static void send(asio::ssl::stream<tcp::socket> &socket, std::string data);
    static std::string recv(asio::ssl::stream<tcp::socket> &socket);

    static error error_for_system_error(std::system_error &ex);
};

};  // namespace client
};  // namespace quesync
