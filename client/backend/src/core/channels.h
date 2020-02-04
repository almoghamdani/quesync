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
    /**
     * Channels module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    channels(std::shared_ptr<client> client);

    /**
     * Get a private channel with a user.
     *
     * @param user_id The id of the user.
     * @return A shared pointer to the channel object.
     */
    std::shared_ptr<channel> get_private_channel(std::string user_id);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync