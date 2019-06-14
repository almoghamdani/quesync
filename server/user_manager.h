#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <unordered_map>

#include "../shared/friend_request.h"
#include "../shared/user.h"
#include "../shared/profile.h"

// Prevent loop header include
class Session;

class UserManager : Manager
{
public:
    UserManager(std::shared_ptr<Quesync> server);

    bool doesUserExists(std::string user_id);

    std::shared_ptr<User> registerUser(std::shared_ptr<Session> sess, std::string username, std::string password, std::string email, std::string nickname);
    std::shared_ptr<User> authenticateUser(std::shared_ptr<Session> sess, std::string username, std::string password);
    std::shared_ptr<Session> getAuthenticatedSessionOfUser(std::string user_id);
    void unauthenticateSession(std::string user_id);

    std::shared_ptr<Profile> getUserProfile(std::string id);

    void sendFriendRequest(std::string requester_id, std::string recipient_id);
    void setFriendshipStatus(std::string requester_id, std::string friend_id, bool status);

private:
    sql::Table users_table;
    sql::Table friendships_table;
    sql::Table profiles_table;

    std::unordered_map<std::string, std::weak_ptr<Session>> _authenticated_sessions;

    std::vector<std::string> getFriends(std::string user_id);
    std::vector<FriendRequest> getFriendRequests(std::string user_id);
};