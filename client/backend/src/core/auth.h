#pragma once
#include "module.h"

#include <string>

#include "../../../../shared/user.h"

namespace quesync {
namespace client {
namespace modules {
class auth : public module {
   public:
    /**
     * Auth module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    auth(std::shared_ptr<client> client);

    /**
     * Authenticate using username and password.
     *
     * @param username The username to authenticate with.
     * @param password The password to authenticate with.
     * @return The session id.
     */
    std::string login(std::string username, std::string password);

    /**
     * Signup to Quesync and authenticate using the new user.
     *
     * @param username The username to signup with.
     * @param password The password to signup with.
     * @param email The email to signup with.
     * @return The session id.
     */
    std::string signup(std::string username, std::string password, std::string email);

    /**
     * Authenticate using session id.
     *
     * @param session_id The session id to authenticate with.
     */
    void session_auth(std::string session_id);

    /**
     * Logout.
     */
    void logout();

    /**
     * Get the current authenticated user.
     *
     * @return A shared pointer to the user object.
     */
    std::shared_ptr<quesync::user> get_user();

    /**
     * Get the session id.
     *
     * @return The session id.
     */
    std::string get_session_id();

    virtual void logged_out();

   private:
    /// A shared pointer to the authenticated user.
    std::shared_ptr<quesync::user> _user;

    /// The session id.
    std::string _session_id;
};
};  // namespace modules
};  // namespace client
};  // namespace quesync