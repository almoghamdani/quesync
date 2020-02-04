#pragma once

#ifdef _WIN32
#include <malloc.h>  // Fix _alloca issue with asio
#endif

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <string>
#include <thread>

#include "../../../../shared/error.h"
#include "../../../../shared/header.h"

#define MAX_PATH_LEN 1024

using asio::ip::tcp;
using asio::ip::udp;

namespace quesync {
namespace client {
class socket_manager {
   public:
    /// The I/O context that handles sync and async operations.
    static asio::io_context io_context;

    /// The SSL/TLS context that handles stream encryption.
    static asio::ssl::context ssl_context;

    /**
     * Initializes the socket manager.
     */
    static void init();

    /**
     * Generates an endpoint for an IP address.
     *
     * @tparam T The endpoint type.
     * @param ip_address The IP address of the endpoint.
     * @param port The port of the endpoint.
     * @param endpoint A reference to the endpoint object.
     */
    template <typename T>
    static void get_endpoint(const char *ip_address, int port, T &endpoint);

    /**
     * Sends a packet to an encrypted socket.
     *
     * @param socket The socket to send the packet to.
     * @param data The data of the packet to send to the server.
     */
    static void send(asio::ssl::stream<tcp::socket> &socket, std::string data);

    /**
     * Receive a packet from a socket.
     *
     * @param socket The socket to get a packet from.
     * @return The data of the packet received.
     */
    static std::string recv(asio::ssl::stream<tcp::socket> &socket);

    /**
     * Generates error object for ASIO system error.
     *
     * @param ex The system error of ASIO.
     * @return The generated error object.
     */
    static error error_for_system_error(std::system_error &ex);

   private:
    static std::string get_binary_path();
};
};  // namespace client
};  // namespace quesync
