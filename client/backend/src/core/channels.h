#pragma once
#include "module.h"

#include <string>
#include <vector>

#include "../../../../shared/channel.h"

namespace quesync {
namespace client {
namespace modules {
class channels : public module {
   public:
    channels(std::shared_ptr<client> client);

    std::shared_ptr<channel> get_private_channel(std::string user_id);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync