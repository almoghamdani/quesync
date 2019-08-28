#include "session_manager.h"

#include <ctime>
#include <sole.hpp>

#include "server.h"
#include "session.h"
#include "user_manager.h"

#include "../shared/exception.h"

quesync::server::session_manager::session_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server), sessions_table(server->db(), "sessions") {}

std::string quesync::server::session_manager::create_session(
    std::shared_ptr<quesync::server::session> sess) {
    std::string session_id = sole::uuid4().str();

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    try {
        // Try to insert the new session to the database
        sessions_table.insert("session_id", "user_id")
            .values(session_id, sess->user()->id)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    return session_id;
}

std::string quesync::server::session_manager::get_user_id_for_session(std::string session_id) {
    sql::Row res;

    try {
        // Try to get the user id from the sessions table using the session id
        res = sessions_table.select("user_id")
                  .where("session_id = :session_id")
                  .bind("session_id", session_id)
                  .execute()
                  .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If no session was found, throw error
    if (res.isNull()) {
        throw exception(error::invalid_session);
    }

    return (std::string)res[0];
}

void quesync::server::session_manager::destroy_session(
    std::shared_ptr<quesync::server::session> sess, std::string session_id) {
    try {
        // Try to remove the session from the sessions table
        sessions_table.remove()
            .where("session_id = :session_id")
            .bind("session_id", session_id)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Unauthenticate the user
    _server->user_manager()->unauthenticate_session(sess->user()->id);

    // Clear the user object in the session object
    sess->set_user(nullptr);
}