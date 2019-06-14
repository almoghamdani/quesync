#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <vector>

#include "../shared/message.h"

#define MAX_MESSAGES_AMOUNT 250

// Prevent loop header include
class Session;

class MessageManager : Manager
{
public:
    MessageManager(std::shared_ptr<Quesync> server);

	std::shared_ptr<Message> sendMessage(std::shared_ptr<Session> sess, std::string content, std::string channel_id);

	std::vector<Message> getMessages(std::shared_ptr<Session> sess, std::string channel_id, unsigned int offset, unsigned int amount);

private:
	sql::Table messages_table;
};