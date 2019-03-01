#pragma once

#include <asio.hpp>
#include <map>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "user_management.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync
{
public:
    Quesync(asio::io_context& io_context);
    ~Quesync();

    void start();

    UserManagement& userManagement();
    sql::Schema& db();

private:
    tcp::acceptor _acceptor;

    sql::Session _sess;
    sql::Schema _db;

    UserManagement _userManagement;

    void acceptClient();
};