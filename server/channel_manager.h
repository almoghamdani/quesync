#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <unordered_map>

#include "../shared/friend_request.h"
#include "../shared/user.h"
#include "../shared/channel.h"

// Prevent loop header include
class Session;

class ChannelManager : Manager
{
public:
    ChannelManager(std::shared_ptr<Quesync> server);

    std::shared_ptr<Channel> getPrivateChannel(std::shared_ptr<Session> sess, std::string participant_id);

private:
    sql::Table users_table;
    sql::Table channels_table;
};