#pragma once
#include "manager.h"

#include "../shared/user.h"

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class session_manager : manager {
   public:
    session_manager(std::shared_ptr<server> server);

    std::string create_session(std::shared_ptr<session> sess);
    std::string get_user_id_for_session(std::string session_id);
    void destroy_session(std::shared_ptr<session> sess);
};
}  // namespace server
}  // namespace quesync
