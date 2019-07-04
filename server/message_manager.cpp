#include "message_manager.h"

#include <ctime>
#include <sole.hpp>

#include "quesync.h"
#include "session.h"
#include "event_manager.h"

#include "../shared/quesync_exception.h"
#include "../shared/events/message_event.h"

MessageManager::MessageManager(std::shared_ptr<Quesync> server) : Manager(server),
																  messages_table(server->db(), "messages")
{
}

std::shared_ptr<Message> MessageManager::sendMessage(std::shared_ptr<Session> sess, std::string content, std::string channel_id)
{
	std::shared_ptr<Message> message;
	std::string message_id = sole::uuid4().str();
	std::vector<std::string> channel_members;

	MessageEvent message_evt;

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

	try
	{
		// Try to insert the new message to the database
		messages_table
			.insert("id", "sender_id", "channel_id", "content")
			.values(message_id, sess->user()->id(), channel_id, content)
			.execute();
	}
	catch (...)
	{
		throw QuesyncException(UNKNOWN_ERROR);
	}

	// Create the message object
	message = std::make_shared<Message>(message_id, sess->user()->id(), channel_id, content, std::time(nullptr));

	// Create the event to be sent to the other online users
	message_evt = MessageEvent(*message);

	// Get the other channel members
	channel_members = _server->channelManager()->getChannelMembers(sess, channel_id);

	// For each other channel member
	for (std::string &member_id : channel_members)
	{
		// Trigger the event in the other member
		_server->eventManager()->triggerEvent(message_evt, member_id);
	}

	return message;
}

std::vector<Message> MessageManager::getMessages(std::shared_ptr<Session> sess, std::string channel_id, unsigned int amount, unsigned int offset)
{
	std::vector<Message> messages;

	sql::RowResult res;
	sql::Row row;

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

	// Check if amount surpasses the limit
	if (amount > MAX_MESSAGES_AMOUNT)
	{
		throw QuesyncException(AMOUNT_EXCEEDED_MAX);
	}

	try
	{
		// Try to get the messages from the table using the amount and offset
		res = messages_table
				  .select("id", "sender_id", "channel_id", "content", "unix_timestamp(sent_at)")
				  .where("channel_id = :channel_id")
				  .orderBy("sent_at DESC")
				  .limit(amount)
				  .offset(offset)
				  .bind("channel_id", channel_id)
				  .execute();
	}
	catch (...)
	{
		throw QuesyncException(UNKNOWN_ERROR);
	}

	// For each row in the result, create a message
	while ((row = res.fetchOne()))
	{
		// Create the message from the row
		messages.push_back(Message(((std::string)row[0]).c_str(),
								   ((std::string)row[1]).c_str(),
								   ((std::string)row[2]).c_str(),
								   ((std::string)row[3]).c_str(),
								   row[4]));
	}

	return messages;
}
