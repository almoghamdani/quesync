#pragma once
#include <asio.hpp>

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync
{
public:
    Quesync(asio::io_context& io_context);

    void start();

private:
    tcp::acceptor _acceptor;

    void acceptClient();
};