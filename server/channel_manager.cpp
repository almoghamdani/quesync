#include "channel_manager.h"

#include <ctime>
#include <sole.hpp>

#include "quesync.h"
#include "session.h"
#include "event_manager.h"

#include "../shared/quesync_exception.h"

ChannelManager::ChannelManager(std::shared_ptr<Quesync> server) : Manager(server),
                                                                  users_table(server->db(), "users"),
                                                                  channels_table(server->db(), "channels")
{
}

std::shared_ptr<Channel> ChannelManager::getPrivateChannel(std::shared_ptr<Session> sess, std::string participant_id)
{
    sql::Row channel_res;

    // Check if the session is authenticated
    if (!sess->authenticated())
    {
        throw QuesyncException(NOT_AUTHENTICATED);
    }

    // Check if the other participant exists
    if (!users_table.select("1").where("id = :id").bind("id", participant_id).execute().count())
    {
        throw QuesyncException(USER_NOT_FOUND);
    }
}
