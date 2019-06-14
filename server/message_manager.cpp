#include "message_manager.h"

#include <ctime>
#include <sole.hpp>

#include "quesync.h"
#include "session.h"
#include "event_manager.h"

#include "../shared/quesync_exception.h"

MessageManager::MessageManager(std::shared_ptr<Quesync> server) : Manager(server),
																  messages_table(server->db(), "messages")
{
}

std::shared_ptr<Message> MessageManager::sendMessage(std::shared_ptr<Session> sess, std::string content, std::string channel_id)
{
    std::shared_ptr<Message> message;
    std::string message_id = sole::uuid4().str();

	// Check if the session is authenticated
    if (!sess->authenticated())
    {
        throw QuesyncException(NOT_AUTHENTICATED);
    }

	// Check if the channel exists
    if (!_server->channelManager()->doesChannelExists(channel_id))
    {
        throw QuesyncException(CHANNEL_NOT_FOUND);
    }

    // Check if the user is a member of the wanted channel
    if (!_server->channelManager()->isUserMemberOfChannel(sess->user()->id(), channel_id))
    {
        throw QuesyncException(NOT_MEMBER_OF_CHANNEL);
    }

    try {
        // Try to insert the new message to the database
        messages_table
            .insert("id", "sender_id", "channel_id", "content")
            .values(message_id, sess->user()->id(), channel_id, content)
            .execute();
    } catch (...) {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // Create the message object and return it
    message = std::make_shared<Message>(message_id, sess->user()->id(), channel_id, content, std::time(nullptr));
    return message;
}