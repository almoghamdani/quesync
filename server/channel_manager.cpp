#include "channel_manager.h"

#include <ctime>
#include <sole.hpp>

#include "quesync.h"
#include "session.h"

#include "../shared/quesync_exception.h"

ChannelManager::ChannelManager(std::shared_ptr<Quesync> server) : Manager(server),
                                                                  channels_table(server->db(), "channels"),
                                                                  channel_participants_table(server->db(), "channel_participants")
{
}

bool ChannelManager::doesChannelExists(std::string channel_id)
{
    try
    {
        return channels_table
            .select("1")
            .where("id = :channel_id")
            .bind("channel_id", channel_id)
            .execute()
            .count();
    }
    catch (...)
    {
        return false;
    }
}

std::shared_ptr<Channel> ChannelManager::getPrivateChannel(std::shared_ptr<Session> sess, std::string participant_id)
{
    std::shared_ptr<Channel> channel;
    sql::Row channel_res;

    // Check if the session is authenticated
    if (!sess->authenticated())
    {
        throw QuesyncException(NOT_AUTHENTICATED);
    }

    // Check if the other participant exists
    if (!_server->userManager()->doesUserExists(participant_id))
    {
        throw QuesyncException(USER_NOT_FOUND);
    }

    // Try to get the private channel of the 2 users
    try
    {
        channel_res = _server->db()
                          .getSession()
                          .sql("CALL get_private_channel(?, ?);")
                          .bind(sess->user()->id())
                          .bind(participant_id)
                          .execute()
                          .fetchOne();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // If the channel is null create the channel
    if (channel_res.isNull())
    {
        // Create a channel for the users
        channel = createChannel(true);

        // Add the users as a participants in the channel
        addParticipantToChannel(channel->id(), sess->user()->id());
        addParticipantToChannel(channel->id(), participant_id);
    }
    else
    {
        // Get the channel
        channel = getChannel(channel_res[0]);
    }

    return channel;
}

bool ChannelManager::isUserParticipantOfChannel(std::string user_id, std::string channel_id)
{
    try {
        return channel_participants_table
            .select("1")
            .where("channel_id = :channel_id AND user_id = :user_id")
            .bind("channel_id", channel_id)
            .bind("user_id", user_id)
            .execute()
            .count();
    } catch (...) {
        return false;
    }
}

void ChannelManager::addParticipantToChannel(std::string channel_id, std::string participant_id)
{
    // If the channel is not found, throw error
    if (!doesChannelExists(channel_id))
    {
        throw QuesyncException(CHANNEL_NOT_FOUND);
    }

    // If the participant already exists, throw error
    if (isUserParticipantOfChannel(participant_id, channel_id))
    {
        throw QuesyncException(ALREADY_PARTICIPANT);
    }

    try
    {
        // Add the user as a participant in the room
        channel_participants_table
            .insert("channel_id", "user_id")
            .values(channel_id, participant_id)
            .execute();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }
}

std::shared_ptr<Channel> ChannelManager::getChannel(std::string channel_id)
{
    std::shared_ptr<Channel> channel;
    sql::Row channel_res;

    // Try to get the private channel of the 2 users
    try
    {
        channel_res = channels_table
                          .select("id", "is_private", "unix_timestamp(created_at)")
                          .where("id = :channel_id")
                          .bind("channel_id", channel_id)
                          .execute()
                          .fetchOne();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // If the channel res is null, return null
    if (channel_res.isNull())
    {
        throw QuesyncException(CHANNEL_NOT_FOUND);
    }

    // Create the channel object and return it
    channel = std::make_shared<Channel>(channel_res[0], channel_res[1], (int)channel_res[2]);
    return channel;
}

std::shared_ptr<Channel> ChannelManager::createChannel(bool is_private)
{
    std::shared_ptr<Channel> channel;
    std::string channel_id = sole::uuid4().str();

    try
    {
        // Try to add the new channel
        channels_table
            .insert("id", "is_private")
            .values(channel_id, is_private)
            .execute();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // Create the channel object
    channel = std::make_shared<Channel>(channel_id, is_private, std::time(nullptr));

    return channel;
}
