#pragma once
#include "manager.h"

#include <vector>

#include "../../shared/message.h"

#define MAX_MESSAGES_AMOUNT 250

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class message_manager : manager {
   public:
    /**
     * Message maanger constructor.
     *
     * @param server A shared pointer to the server object.
     */
    message_manager(std::shared_ptr<server> server);

    /**
     * Send a message to a channel.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param content The content of the message.
     * @param attachment_id The id of attachment to attach to the message. This can be null.
     * @param channel_id The id of the channel.
     * @return A shared pointer to the generated message object.
     */
    std::shared_ptr<message> send_message(std::shared_ptr<session> sess, std::string content,
                                          std::string attachment_id, std::string channel_id);

    /**
     * Gets a list of messages from a channel.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param channel_id The id of the channel.
     * @param amount The amount of messages to request from the server.
     * @param offset The offset in the list of messages.
     * @return A vector containing the messages from the channel.
     */
    std::vector<message> get_messages(std::shared_ptr<session> sess, std::string channel_id,
                                      unsigned int amount, unsigned int offset);
};
};  // namespace server
};  // namespace quesync
