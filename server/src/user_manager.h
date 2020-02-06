#pragma once
#include "manager.h"

#include <mutex>
#include <unordered_map>
#include <vector>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../../shared/friend_request.h"
#include "../../shared/profile.h"
#include "../../shared/user.h"

#define PHOTO_FILE_MAX_SIZE 5 * 1000000
namespace quesync {
namespace server {
// Prevent loop header include
class session;

class user_manager : manager {
   public:
    /**
     * User manager constructor.
     *
     * @param server A shared pointer to the server object.
     */
    user_manager(std::shared_ptr<server> server);

    /**
     * Checks if a user exists
     *
     * @param user_id The id of the user.
     * @return True if the user exists or false otherwise.
     */
    bool does_user_exists(std::string user_id);

    /**
     * Registers a user to Quesync.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param username The username of the new user.
     * @param password The password of the new user.
     * @param email The email of the new user.
     * @param nickname The nickname of the new user.
     * @return A shared pointer of the new user object.
     */
    std::shared_ptr<user> register_user(std::shared_ptr<quesync::server::session> sess,
                                        std::string username, std::string password,
                                        std::string email, std::string nickname);

    /**
     * Authenticate a client using a session id.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param session_id The id of the session.
     * @return A shared pointer of the new user object.
     */
    std::shared_ptr<user> authenticate_user_by_session(
        std::shared_ptr<quesync::server::session> sess, std::string session_id);

    /**
     * Authenticate a client using username and password.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param username The username of the user.
     * @param password The password of the user.
     * @return A shared pointer of the new user object.
     */
    std::shared_ptr<user> authenticate_user(std::shared_ptr<quesync::server::session> sess,
                                            std::string username, std::string password);

    /**
     * Gets the session of an authenticated user.
     *
     * @param user_id The id of the user.
     * @return A shared pointer to the session object of the user.
     */
    std::shared_ptr<quesync::server::session> get_authenticated_session_of_user(
        std::string user_id);

    /**
     * Unauthenticate a user's session.
     *
     * @param user_id The id of the user.
     */
    void unauthenticate_session(std::string user_id);

    /**
     * Sets the profile photo of a user.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param file_id The id of the profile photo file.
     */
    void set_profile_photo(std::shared_ptr<quesync::server::session> sess, std::string file_id);

    /**
     * Get the user profile of a user.
     *
     * @param id The id of the user.
     * @return A shared pointer to the user's profile object.
     */
    std::shared_ptr<profile> get_user_profile(std::string id);

    /**
     * Sends a friend request to a user.
     *
     * @param requester_id The id of the requester.
     * @param recipient_id The id of the recipient.
     */
    void send_friend_request(std::string requester_id, std::string recipient_id);

    /**
     * Sets the friendship status between 2 users.
     *
     * @param requester_id The id of the requester.
     * @param friend_id The id of the friend.
     * @param status The new friendship status.
     */
    void set_friendship_status(std::string requester_id, std::string friend_id, bool status);

    /**
     * Search a user by nickname and optional tag.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param nickname The nickname of the user.
     * @param tag The tag of the user.
     * @return A vector containing the search results.
     */
    std::vector<profile> search(std::shared_ptr<quesync::server::session> sess,
                                std::string nickname, int tag);

   private:
    /// A map of authenticated sessions.
    std::unordered_map<std::string, std::weak_ptr<session>> _authenticated_sessions;

    /// Authenticated sessions lock.
    std::mutex _sessions_mutex;

    std::string get_profile_photo(std::string photo_id);

    std::vector<std::string> get_friends(sql::Session &sql_sess, std::string user_id);
    std::vector<friend_request> get_friend_requests(sql::Session &sql_sess, std::string user_id);
};
};  // namespace server
};  // namespace quesync
