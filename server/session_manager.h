#pragma once
#include "manager.h"

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../shared/user.h"

// Prevent loop header include
class Session;

class SessionManager : Manager
{
public:
    SessionManager(std::shared_ptr<Quesync> server);

	std::string createSession(std::shared_ptr<Session> sess);
	std::string getUserIdForSession(std::string session_id);
	void destroySession(std::shared_ptr<Session> sess, std::string session_id);

private:
	sql::Table sessions_table;
};