#pragma once
#include "manager.h"

#include <vector>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../../shared/channel.h"

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class channel_manager : manager {
   public:
    channel_manager(std::shared_ptr<server> server);

    bool does_channel_exists(std::string channel_id);

    std::shared_ptr<channel> get_private_channel(std::shared_ptr<session> sess,
                                                 std::string user_id);
    std::shared_ptr<channel> get_channel(std::string channel_id);

    bool is_user_member_of_channel(std::string user_id, std::string channel_id);
    void add_member_to_channel(std::string channel_id, std::string member_id);
    std::vector<std::string> get_channel_members(std::shared_ptr<session> sess,
                                                 std::string channel_id);

   private:
    std::shared_ptr<channel> create_channel(sql::Session &sql_sess, bool is_private);
};
};  // namespace server
};  // namespace quesync
