#pragma once
#include <memory>
#include <asio.hpp>

#include "quesync.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, Quesync *server);

    void start();

private:
    tcp::socket _socket;
    Quesync *_server;
};