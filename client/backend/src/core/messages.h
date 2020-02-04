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
    /**
     * Messages module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    messages(std::shared_ptr<client> client);

    /**
     * Sends a message to a channel.
     *
     * @param content The content of the message.
     * @param channel_id The id of the channel to send the message to.
     * @param attachment_id The id of attachment to attach to the message. This can be null.
     * @return A shared pointer to the generated message object.
     */
    std::shared_ptr<message> send_message(std::string content, std::string channel_id,
                                          std::string attachment_id = nullptr);

    /**
     * Gets a list of messages from a channel.
     *
     * @param channel_id The id of the channel.
     * @param amount The amount of messages to request from the server.
     * @param offset The offset in the list of messages.
     * @return A vector containing the messages from the channel.
     */
    std::vector<message> get_channel_messages(std::string channel_id, unsigned int amount,
                                              unsigned int offset);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync