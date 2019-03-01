#pragma once

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../shared/user.h"

class UserManagement 
{
public:
    UserManagement(sql::Schema& db);

    User *registerUser(std::string username, std::string password, std::string email, std::string nickname);
    User *authenticateUser(std::string username, std::string password);

private:
    sql::Table users_table;
    sql::Table friendships_table;
};