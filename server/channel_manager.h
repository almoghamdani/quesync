#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <vector>

#include "../shared/channel.h"

// Prevent loop header include
class Session;

class ChannelManager : Manager
{
public:
    ChannelManager(std::shared_ptr<Quesync> server);

    bool doesChannelExists(std::string channel_id);

    std::shared_ptr<Channel> getPrivateChannel(std::shared_ptr<Session> sess, std::string user_id);
    std::shared_ptr<Channel> getChannel(std::string channel_id);

    bool isUserMemberOfChannel(std::string user_id, std::string channel_id);
    void addMemberToChannel(std::string channel_id, std::string member_id);
    std::vector<std::string> getChannelMembers(std::shared_ptr<Session> sess, std::string channel_id);

private:
    sql::Table channel_members_table;
    sql::Table channels_table;

    std::shared_ptr<Channel> createChannel(bool is_private);
};