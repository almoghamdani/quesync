#pragma once
#include "manager.h"

#include <mutex>
#include <unordered_map>
#include <vector>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../shared/friend_request.h"
#include "../shared/profile.h"
#include "../shared/user.h"

#define PHOTO_FILE_MAX_SIZE 5 * 1000000
namespace quesync {
namespace server {
// Prevent loop header include
class session;

class user_manager : manager {
   public:
    user_manager(std::shared_ptr<server> server);

    bool does_user_exists(std::string user_id);

    std::shared_ptr<user> register_user(std::shared_ptr<quesync::server::session> sess,
                                        std::string username, std::string password,
                                        std::string email, std::string nickname);
    std::shared_ptr<user> authenticate_user_by_session(
        std::shared_ptr<quesync::server::session> sess, std::string session_id);
    std::shared_ptr<user> authenticate_user(std::shared_ptr<quesync::server::session> sess,
                                            std::string username, std::string password);
    std::shared_ptr<quesync::server::session> get_authenticated_session_of_user(
        std::string user_id);
    void unauthenticate_session(std::string user_id);

    void set_profile_photo(std::shared_ptr<quesync::server::session> sess, std::string file_id);

    std::shared_ptr<profile> get_user_profile(std::string id);

    void send_friend_request(std::string requester_id, std::string recipient_id);
    void set_friendship_status(std::string requester_id, std::string friend_id, bool status);

    std::vector<profile> search(std::shared_ptr<quesync::server::session> sess,
                                std::string nickname, int tag);

   private:
    std::mutex _sessions_mutex;
    std::unordered_map<std::string, std::weak_ptr<session>> _authenticated_sessions;

    std::string get_profile_photo(std::string photo_id);

    std::vector<std::string> get_friends(sql::Session &sql_sess, std::string user_id);
    std::vector<friend_request> get_friend_requests(sql::Session &sql_sess, std::string user_id);
};
};  // namespace server
};  // namespace quesync
