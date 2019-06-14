#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <unordered_map>

#include "../shared/message.h"

// Prevent loop header include
class Session;

class MessageManager : Manager
{
public:
    MessageManager(std::shared_ptr<Quesync> server);

	std::shared_ptr<Message> sendMessage(std::shared_ptr<Session> sess, std::string content, std::string channel_id);

private:
	sql::Table messages_table;
};