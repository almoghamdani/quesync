#pragma once
#include "module.h"

#include <string>
#include <vector>

#include "../../../../shared/friend_request.h"
#include "../../../../shared/profile.h"

namespace quesync {
namespace client {
namespace modules {
class users : public module {
   public:
    users(std::shared_ptr<client> client);

    std::shared_ptr<profile> get_user_profile(std::string user_id);
    std::vector<profile> search(std::string nickname, int tag = -1);
    std::shared_ptr<friend_request> send_friend_request(std::string friend_id);
    void set_friendship_status(std::string friend_id, bool status);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync