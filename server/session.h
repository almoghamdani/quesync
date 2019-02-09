#pragma once
#include <memory>
#include <asio.hpp>

#include "quesync.h"

#define MAX_DATA_LEN 1000

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, Quesync *server);

    void start();
    void recv();

private:
    tcp::socket _socket;
    Quesync *_server;
    char _data[MAX_DATA_LEN];
};