#include "channel_management.h"

#include <ctime>
#include <sole.hpp>

#include "session.h"
#include "event_management.h"

ChannelManagement::ChannelManagement(sql::Schema &db) : users_table(db, "users"),
                                                        channels_table(db, "channels")
{
}

std::shared_ptr<Channel> ChannelManagement::getPrivateChannel(std::shared_ptr<Session> sess, std::string participant_id)
{
    // Check if the other participant exists
    if (!users_table.select("1").where("id = :id").bind("id", participant_id).execute().count())
    {
        throw QuesyncException(USER_NOT_FOUND);
    }

    
}
