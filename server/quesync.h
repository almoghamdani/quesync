#pragma once
#include <asio.hpp>

#include <map>

#ifdef _MSC_VER
#pragma warning(disable: 4267) // disable warning 4267
#endif
#include <sqlitepp.h>
#ifdef _MSC_VER
#pragma warning(default: 4267) // enable warning 4267 back
#endif

#include "../shared/user.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync
{
public:
    Quesync(asio::io_context& io_context);
    ~Quesync();

    void start();

    User *registerUser(std::string username, std::string password, std::string email, std::string nickname);
    User *authenticateUser(std::string username, std::string password);

private:
    tcp::acceptor _acceptor;
    sqlitepp::db *_db;

    void initDB();
    void acceptClient();
};