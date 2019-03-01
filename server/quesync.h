#pragma once
#include <asio.hpp>

#include <map>
#include <mysqlx/xdevapi.h>

#include "../shared/user.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

namespace sql = mysqlx;

class Quesync
{
public:
    Quesync(asio::io_context& io_context);
    ~Quesync();

    void start();

    User *registerUser(std::string username, std::string password, std::string email, std::string nickname);
    User *authenticateUser(std::string username, std::string password);

    sql::Schema& db();

private:
    tcp::acceptor _acceptor;

    sql::Session _sess;
    sql::Schema _db;

    void initDB();
    void acceptClient();
};