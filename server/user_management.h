#pragma once

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <unordered_map>

#include "session.h"

#include "../shared/user.h"
#include "../shared/profile.h"

class UserManagement
{
public:
  UserManagement(sql::Schema &db);

  User *registerUser(std::shared_ptr<Session> sess, std::string username, std::string password, std::string email, std::string nickname);
  User *authenticateUser(std::shared_ptr<Session> sess, std::string username, std::string password);

  Profile *getUserProfile(std::string id);  

  void sendFriendRequest(std::string requester_id, std::string recipient_id);
  void setFriendshipStatus(std::string requester_id, std::string friend_id, bool status);

private:
  sql::Table users_table;
  sql::Table friendships_table;
  sql::Table profiles_table;

  std::unordered_map<std::string, std::shared_ptr<Session>> _authenticated_sessions;

  std::vector<std::string> getFriends(std::string user_id);
};