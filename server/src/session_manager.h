#pragma once
#include "manager.h"

#include "../../shared/user.h"

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class session_manager : manager {
   public:
    /**
     * Session maanger constructor.
     *
     * @param server A shared pointer to the server object.
     */
    session_manager(std::shared_ptr<server> server);

    /**
     * Create a session for a user.
     *
     * @param sess A shared pointer to the session object of the user.
     * @return The session id for the user.
     */
    std::string create_session(std::shared_ptr<session> sess);

    /**
     * Get the user for a session.
     *
     * @param session_id The session id.
     * @return The id of the user that is associated with the session.
     */
    std::string get_user_id_for_session(std::string session_id);

    /**
     * Destroy a user's session.
     *
     * @param sess A shared pointer to the session object of the user.
     */
    void destroy_session(std::shared_ptr<session> sess);
};
}  // namespace server
}  // namespace quesync
