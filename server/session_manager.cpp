#include "session_manager.h"

#include <ctime>
#include <sole.hpp>

#include "quesync.h"
#include "session.h"
#include "user_manager.h"

#include "../shared/quesync_exception.h"

SessionManager::SessionManager(std::shared_ptr<Quesync> server) : Manager(server),
																  sessions_table(server->db(), "sessions")
{
}

std::string SessionManager::createSession(std::shared_ptr<Session> sess)
{
	std::string session_id = sole::uuid4().str();

	// Check if the session is authenticated
	if (!sess->authenticated())
	{
		throw QuesyncException(NOT_AUTHENTICATED);
	}

	try
	{
		// Try to insert the new session to the database
		sessions_table
			.insert("session_id", "user_id")
			.values(session_id, sess->user()->id())
			.execute();
	}
	catch (...)
	{
		throw QuesyncException(UNKNOWN_ERROR);
	}

	return session_id;
}

std::string SessionManager::getUserIdForSession(std::string session_id) {
	sql::Row res;
	
	try
    {
        // Try to get the user id from the sessions table using the session id
        res = sessions_table
                  .select("user_id")
                  .where("session_id = :session_id")
                  .bind("session_id", session_id)
                  .execute()
				  .fetchOne();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

	// If no session was found, throw error
	if (res.isNull())
	{
		throw QuesyncException(INVALID_SESSION);
	}

	return res[0];
}

void SessionManager::destroySession(std::shared_ptr<Session> sess, std::string session_id)
{
	try {
		// Try to remove the session from the sessions table
		sessions_table
			.remove()
			.where("session_id = :session_id")
			.bind("session_id", session_id)
			.execute();
	} catch (...) {
		throw QuesyncException(UNKNOWN_ERROR);
	}

	// Unauthenticate the user
	_server->userManager()->unauthenticateSession(sess->user()->id());

	// Clear the user object in the session object
	sess->setUser(nullptr);
}