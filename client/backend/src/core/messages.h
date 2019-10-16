#pragma once
#include "module.h"

#include <string>
#include <vector>

#include "../../../../shared/message.h"

namespace quesync {
namespace client {
namespace modules {
class messages : public module {
   public:
    messages(std::shared_ptr<client> client);

    std::shared_ptr<message> send_message(std::string content, std::string channel_id,
                                          std::string attachment_id = nullptr);
    std::vector<message> get_channel_messages(std::string channel_id, unsigned int amount,
                                              unsigned int offset);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync