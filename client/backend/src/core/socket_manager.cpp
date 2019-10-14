#include "socket_manager.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/utils/parser.h"

// Declare the io context of the socket mananger since it's a static member of the class
asio::io_context quesync::client::socket_manager::io_context;
asio::ssl::context quesync::client::socket_manager::ssl_context(asio::ssl::context::sslv23);

void quesync::client::socket_manager::init() { ssl_context.load_verify_file("cert.pem"); }

template <typename T>
void quesync::client::socket_manager::get_endpoint(const char *ip_address, int port, T &endpoint) {
    // Create an endpoint from the ip address given and the port
    endpoint = T(asio::ip::address::from_string(ip_address), port);
}

// Declaring 2 templates for the GetEndpoint for the 2 possible classes of endpoints to fix link
// errors
template void quesync::client::socket_manager::get_endpoint(const char *ip_address, int port,
                                                            udp::endpoint &endpoint);
template void quesync::client::socket_manager::get_endpoint(const char *ip_address, int port,
                                                            tcp::endpoint &endpoint);

void quesync::client::socket_manager::send(asio::ssl::stream<tcp::socket> &socket,
                                           std::string data) {
    header header = {1, 0};
    std::string full_packet;
    error error;

    // Set data size
    header.size = data.size();

    // Combine the header and the packet
    full_packet = utils::parser::encode_header(header) + data;

    try {
        // Send the server the data
        asio::write(socket, asio::buffer(full_packet.c_str(), full_packet.size()));
    } catch (std::system_error &ex) {
        // Get Quesync error code by the system error
        throw exception(socket_manager::error_for_system_error(ex));
    } catch (...) {
        throw exception(error::unknown_error);
    }
}

std::string quesync::client::socket_manager::recv(
    asio::ssl::stream<tcp::socket> &socket) {
    header header;
    char *header_buf = new char[sizeof(quesync::header)];

    std::shared_ptr<char> buf;

    try {
        // Get the header from the server
        asio::read(socket, asio::buffer(header_buf, sizeof(quesync::header)));

        // Decode the header
        header = utils::parser::decode_header(header_buf);

        // Create the buffer for the message
        buf = std::shared_ptr<char>(new char[header.size]);

        // Get a response from the server
        asio::read(socket, asio::buffer(buf.get(), header.size));
    } catch (std::system_error &ex) {
        // Get Quesync error code by the system error
        throw exception(socket_manager::error_for_system_error(ex));
    } catch (...) {
        throw exception(error::unknown_error);
    }

    return std::string(buf.get(), header.size);
}

quesync::error quesync::client::socket_manager::error_for_system_error(std::system_error &ex) {
    // Get error code
    int error = ex.code().value();

    // If the server cannot be reached
    if (error == asio::error::not_connected || error == asio::error::connection_aborted ||
        error == asio::error::connection_refused || error == asio::error::host_not_found ||
        error == asio::error::host_unreachable || error == asio::error::fault ||
        error == asio::error::timed_out || error == asio::error::shut_down ||
        error == asio::error::connection_reset) {
        return quesync::error::cannot_reach_server;
    } else if (error == asio::error::network_down || error == asio::error::network_reset ||
               error == asio::error::network_unreachable) {
        return quesync::error::no_connection;

    } else {
        return quesync::error::unknown_error;
    }
}