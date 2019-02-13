#pragma once
#include <asio.hpp>
#include <sqlitepp.h>

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync
{
public:
    Quesync(asio::io_context& io_context);
    ~Quesync();

    void start();

private:
    tcp::acceptor _acceptor;
    sqlitepp::db *_db;

    void initDB();
    void acceptClient();
};