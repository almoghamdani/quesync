#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <vector>

#include "../shared/message.h"

#define MAX_MESSAGES_AMOUNT 250

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class message_manager : manager {
   public:
    message_manager(std::shared_ptr<server> server);

    std::shared_ptr<message> send_message(std::shared_ptr<session> sess, std::string content,
                                          std::string channel_id);

    std::vector<message> get_messages(std::shared_ptr<session> sess, std::string channel_id,
                                      unsigned int amount, unsigned int offset);

   private:
    sql::Table messages_table;
};
};  // namespace server
};  // namespace quesync
