#include "user_manager.h"

#include <ctime>
#include <sole.hpp>

#include "server.h"
#include "session.h"

#include "../shared/events/friend_request_event.h"
#include "../shared/events/friendship_status_event.h"
#include "../shared/exception.h"
#include "../shared/utils/crypto/base64.h"
#include "../shared/utils/crypto/pbkdf2.h"
#include "../shared/utils/rand.h"
#include "../shared/utils/server.h"
#include "../shared/utils/validation.h"

quesync::server::user_manager::user_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server) {}

bool quesync::server::user_manager::does_user_exists(std::string user_id) {
    sql::Session sql_sess = _server->get_sql_session();
    sql::Table users_table(_server->get_sql_schema(sql_sess), "users");

    try {
        // Check if the user exists
        return users_table.select("1").where("id = :id").bind("id", user_id).execute().count();
    } catch (...) {
        return false;
    }
}

std::shared_ptr<quesync::user> quesync::server::user_manager::authenticate_user(
    std::shared_ptr<quesync::server::session> sess, std::string username, std::string password) {
    std::shared_ptr<user> user = nullptr;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table users_table(_server->get_sql_schema(sql_sess), "users");
    sql::Row user_res;

    std::unique_lock lk(_sessions_mutex, std::defer_lock);

    // Search for the user row in the database
    try {
        user_res = users_table.select("*")
                       .where("username = :username")
                       .bind("username", username)
                       .execute()
                       .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the user is not found
    if (user_res.isNull()) {
        throw exception(error::user_not_found);
    }
    // If the password the user entered doesn't match the user's password
    else if (!utils::crypto::pbkdf2::sha512_compare(password, std::string(user_res[2]))) {
        throw exception(error::incorrect_password);
    }

    // Lock the mutex
    lk.lock();

    // If the user is already connected in another location
    if (_authenticated_sessions.count((std::string)user_res[0])) {
        throw exception(error::already_connected_in_other_location);
    }

    // Create the user from the db response
    user = std::make_shared<quesync::user>((std::string)user_res[0], (std::string)user_res[1],
                                           (std::string)user_res[3],
                                           ((std::string)user_res[4]).c_str(), user_res[5],
                                           get_friends(sql_sess, (std::string)user_res[0]),
                                           get_friend_requests(sql_sess, (std::string)user_res[0]));

    // Add the user to the authenticated sessions
    _authenticated_sessions.insert_or_assign((std::string)user_res[0], sess);

    return user;
}

std::shared_ptr<quesync::user> quesync::server::user_manager::register_user(
    std::shared_ptr<quesync::server::session> sess, std::string username, std::string password,
    std::string email, std::string nickname) {
    std::shared_ptr<user> user = nullptr;
    std::shared_ptr<unsigned char> password_salt;
    std::string id, password_hashed;
    int tag;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table users_table(_server->get_sql_schema(sql_sess), "users");
    sql::Row user_res;

    std::unique_lock lk(_sessions_mutex, std::defer_lock);

    // Check if the entered username is a valid username
    if (!utils::validation::is_valid_username(username)) {
        throw exception(error::invalid_username);
    }

    // Check if the entered e-mail is a valid e-mail
    if (!utils::validation::is_valid_email(email)) {
        throw exception(error::invalid_email);
    }

    // Check if the password is below fair
    if (utils::validation::score_password(password) < 2) {
        throw exception(error::password_too_weak);
    }

    // Check if there are any users with the username/email/nickname of the new user
    try {
        user_res = users_table.select("*")
                       .where("username = :username OR email = :email")
                       .bind("username", username)
                       .bind("email", email)
                       .execute()
                       .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If no user found, create the new user
    if (user_res.isNull()) {
        // Create an ID for the user
        id = sole::uuid4().str();

        // Generate a random salt for the password
        password_salt = utils::rand::bytes(PBKDF2_SALT_SIZE);

        // Hash the user's password with random salt
        password_hashed = utils::crypto::pbkdf2::sha512(password, password_salt.get());

        // Generate the random user tag
        tag = utils::server::generate_tag(nickname, users_table);

        try {
            // Insert the new user to the users table
            users_table.insert("id", "username", "password", "email", "nickname", "tag")
                .values(id, username, password_hashed, email, nickname, tag)
                .execute();
        } catch (...) {
            throw exception(error::unknown_error);
        }

        // Create the object for the user
        user = std::make_shared<quesync::user>(id, username, email, nickname, tag,
                                               std::vector<std::string>(),
                                               std::vector<friend_request>());

        // Lock the mutex
        lk.lock();

        // Add the user to the authenticated sessions
        _authenticated_sessions[id] = sess;

        // Unlock the mutex
        lk.unlock();
    } else {
        // If the username is already taken
        if (std::string(user_res[1]) == username) {
            throw exception(error::username_already_in_use);
        }
        // If the email is already taken
        else if (std::string(user_res[3]) == email) {
            throw exception(error::email_already_in_use);
        }
        // We shouldn't get here
        else {
            throw exception(error::unknown_error);
        }
    }

    return user;
}

std::shared_ptr<quesync::profile> quesync::server::user_manager::get_user_profile(
    std::string user_id) {
    std::shared_ptr<profile> profile = nullptr;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table profiles_table(_server->get_sql_schema(sql_sess), "profiles");
    sql::Row profile_res;

    // Search for the profile row in the database
    try {
        profile_res = profiles_table.select("*")
                          .where("id = :user_id")
                          .bind("user_id", user_id)
                          .execute()
                          .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the user's profile is not found
    if (profile_res.isNull()) {
        throw exception(error::user_not_found);
    }

    // Create the profile from the db response
    profile = std::make_shared<quesync::profile>(
        (std::string)profile_res[0], (std::string)profile_res[1], profile_res[2],
        get_profile_photo(profile_res[3].isNull() ? "" : (std::string)profile_res[3]));

    return profile;
}

std::string quesync::server::user_manager::get_profile_photo(std::string photo_id) {
    std::string file_content, photo_base64;

    // If the photo id isn't null
    if (!photo_id.empty()) {
        try {
            // Get the content of the file
            file_content = _server->file_manager()->get_file_content(photo_id);

            // Encode the file's content in base64
            photo_base64 = utils::crypto::base64::encode(file_content);
        } catch (...) {
            // Ignore errors to return null photo
        }
    }

    return photo_base64;
}

std::vector<std::string> quesync::server::user_manager::get_friends(sql::Session &sql_sess,
                                                                    std::string user_id) {
    std::vector<std::string> friends;

    sql::Table friendships_table(_server->get_sql_schema(sql_sess), "friendships");
    sql::RowResult res;
    sql::Row row;

    try {
        // Get the ids of the user's friends
        res = friendships_table.select("requester_id", "recipient_id")
                  .where("(requester_id = :user_id OR recipient_id = :user_id) AND approved = True")
                  .bind("user_id", user_id)
                  .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // For each friend of the user, add it to the list of friends
    while ((row = res.fetchOne())) {
        // If the "requester_id" column is the user's id, get the friend id from the "recipient_id"
        // column
        if ((std::string)row[0] == user_id) {
            friends.push_back((std::string)row[1]);
        } else {
            friends.push_back((std::string)row[0]);
        }
    }

    return friends;
}

std::vector<quesync::friend_request> quesync::server::user_manager::get_friend_requests(
    sql::Session &sql_sess, std::string user_id) {
    std::vector<friend_request> friend_requests;

    sql::Table friendships_table(_server->get_sql_schema(sql_sess), "friendships");
    sql::RowResult res;
    sql::Row row;

    try {
        // Get the ids of the user's friend requests
        res =
            friendships_table.select("requester_id", "recipient_id", "unix_timestamp(sent_at)")
                .where("(requester_id = :user_id OR recipient_id = :user_id) AND approved = False")
                .bind("user_id", user_id)
                .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // For each friend request of the user, add it to the list of friend requests
    while ((row = res.fetchOne())) {
        // If the "requester_id" column is the user's id, get the friend id from the "recipient_id"
        // column
        if ((std::string)row[0] == user_id) {
            friend_requests.push_back({(std::string)row[1], "recipient", (int)row[2]});
        } else {
            friend_requests.push_back({(std::string)row[0], "requester", (int)row[2]});
        }
    }

    return friend_requests;
}

void quesync::server::user_manager::send_friend_request(std::string requester_id,
                                                        std::string recipient_id) {
    std::shared_ptr<events::friend_request_event> friend_request_event(
        std::make_shared<events::friend_request_event>(requester_id, std::time(nullptr)));

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table friendships_table(_server->get_sql_schema(sql_sess), "friendships");

    // Check if the recipient exists
    if (!does_user_exists(recipient_id)) {
        throw exception(error::user_not_found);
    }

    // Check if the 2 users are already friends
    try {
        if (friendships_table.select("1")
                .where("(requester_id = :requester_id AND recipient_id = :recipient_id) OR "
                       "(requester_id = :recipient_id AND recipient_id = :requester_id)")
                .bind("requester_id", requester_id)
                .bind("recipient_id", recipient_id)
                .execute()
                .count()) {
            throw exception(error::already_friends);
        }
    } catch (...) {
        throw exception(error::unknown_error);
    }

    try {
        // Add the friend request to the friendships table (approved field is default to false)
        friendships_table.insert("requester_id", "recipient_id")
            .values(requester_id, recipient_id)
            .execute();

        // Trigger an event in the recipient if online
        _server->event_manager()->trigger_event(
            std::static_pointer_cast<events::friend_request_event>(friend_request_event),
            recipient_id);
    } catch (...) {
        throw exception(error::unknown_error);
    }
}

void quesync::server::user_manager::set_friendship_status(std::string user_id,
                                                          std::string friend_id, bool status) {
    std::shared_ptr<events::friendship_status_event> friendship_status_event(
        std::make_shared<events::friendship_status_event>(user_id, status));

    std::string requester, recipient;
    bool approved = false;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table friendships_table(_server->get_sql_schema(sql_sess), "friendships");
    sql::Row friendship_row;

    // Check if the friend exists
    if (!does_user_exists(friend_id)) {
        throw exception(error::user_not_found);
    }

    // Get the friendship between the user and the friend
    try {
        friendship_row =
            friendships_table.select("requester_id", "recipient_id", "approved")
                .where(
                    "(requester_id = :requester_id AND recipient_id = :recipient_id) OR "
                    "(requester_id = :recipient_id AND recipient_id = :requester_id)")
                .bind("requester_id", user_id)
                .bind("recipient_id", friend_id)
                .execute()
                .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the user doesn't have the friend as his friend throw an error
    if (friendship_row.isNull()) {
        throw exception(error::not_friends);
    }

    // Get the fields from the row for easier usage
    requester = friendship_row[0].operator std::string();
    recipient = friendship_row[1].operator std::string();
    approved = (bool)friendship_row[2];

    // If the user wish to disable a pending friend request or just remove a friend, remove the
    // friendship
    if (status == false) {
        try {
            // Remove the friendship row
            friendships_table.remove()
                .where("requester_id = :requester_id AND recipient_id = :recipient_id")
                .bind("requester_id", requester)
                .bind("recipient_id", recipient)
                .execute();

            // Trigger event for friend
            _server->event_manager()->trigger_event(
                std::static_pointer_cast<quesync::event>(friendship_status_event), friend_id);
        } catch (...) {
            throw exception(error::unknown_error);
        }
    }
    // If the user is the requester and he is trying to self-approve his friend request
    else if (requester == user_id && approved == false) {
        throw exception(error::self_approve_friend_request);
    }
    // If the user is trying to approve a friend request where he is already friends with
    else if (approved == status == true) {
        throw exception(error::already_friends);
    } else {
        try {
            // Set the approved field in the friendship row
            friendships_table.update()
                .set("approved", true)
                .where("requester_id = :requester_id AND recipient_id = :recipient_id")
                .bind("requester_id", requester)
                .bind("recipient_id", recipient)
                .execute();

            // Trigger event for friend
            _server->event_manager()->trigger_event(
                std::static_pointer_cast<quesync::event>(friendship_status_event), friend_id);
        } catch (...) {
            throw exception(error::unknown_error);
        }
    }
}

void quesync::server::user_manager::unauthenticate_session(std::string user_id) {
    std::lock_guard lk(_sessions_mutex);

    // Try to erase the session from the authenticated sessions and silence errors
    try {
        _authenticated_sessions.erase(user_id);
    } catch (...) {
    }
}

void quesync::server::user_manager::set_profile_photo(
    std::shared_ptr<quesync::server::session> sess, std::string file_id) {
    std::shared_ptr<file> photo_file;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table users_table(_server->get_sql_schema(sql_sess), "users");

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // Try to get the file of the photo
    photo_file = _server->file_manager()->get_file_info(file_id);

    // Check for valid size of the photo
    if (photo_file->size > PHOTO_FILE_MAX_SIZE) {
        throw exception(error::profile_photo_too_big);
    }

    try {
        // Set the new profile photo for the user
        users_table.update()
            .set("photo_id", file_id)
            .where("id = :user_id")
            .bind("user_id", sess->user()->id)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }
}

std::shared_ptr<quesync::server::session>
quesync::server::user_manager::get_authenticated_session_of_user(std::string user_id) {
    std::lock_guard lk(_sessions_mutex);

    try {
        // Try to get the authenticated session for the user id
        return _authenticated_sessions.at(user_id).lock();
    } catch (...) {
        // If not found return null
        return nullptr;
    }
}

std::vector<quesync::profile> quesync::server::user_manager::search(
    std::shared_ptr<quesync::server::session> sess, std::string nickname, int tag) {
    std::vector<quesync::profile> results;

    std::string nicknameInitial = nickname;
    std::string nicknameEnd = nickname;

    sql::Session sql_sess = _server->get_sql_session();
    std::vector<sql::Row> res;

    // Insert % before and after the string to match any string containing the nickname
    nickname.insert(0, "%");
    nickname.append("%");

    // Add % at the end to get all nicknames starting with the wanted nickname
    nicknameInitial.append("%");

    // Add % at the end to get all nicknames starting with the wanted nickname
    nicknameEnd.insert(0, "%");

    try {
        // If a tag was entered, select with it as a requirement
        if (tag != -1) {
            // Get all users matching the searched nickname and tag
            res = sql_sess
                      .sql(
                          "SELECT * FROM quesync.profiles WHERE nickname LIKE ? AND "
                          "tag = ? AND id != ? ORDER BY CASE WHEN nickname LIKE ? THEN 0 WHEN "
                          "nickname LIKE ? THEN 1 ELSE 2 END")
                      .bind(nickname)
                      .bind(tag)
                      .bind(sess->user()->id)
                      .bind(nicknameInitial)
                      .bind(nicknameEnd)
                      .execute()
                      .fetchAll();
        } else {
            // Get all users matching the searched nickname
            res = sql_sess
                      .sql(
                          "SELECT * FROM quesync.profiles WHERE nickname LIKE ? AND "
                          "id != ? ORDER BY CASE WHEN nickname LIKE ? THEN 0 WHEN nickname LIKE ? "
                          "THEN 1 ELSE 2 END")
                      .bind(nickname)
                      .bind(sess->user()->id)
                      .bind(nicknameInitial)
                      .bind(nicknameEnd)
                      .execute()
                      .fetchAll();
        }
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Format the results in the json type
    for (int i = 0; i < res.size(); i++) {
        results.push_back(
            profile((std::string)res[i][0], (std::string)res[i][1], res[i][2],
                    get_profile_photo(res[i][3].isNull() ? "" : (std::string)res[i][3])));
    }

    return results;
}

#include <iostream>

std::shared_ptr<quesync::user> quesync::server::user_manager::authenticate_user_by_session(
    std::shared_ptr<quesync::server::session> sess, std::string session_id) {
    std::shared_ptr<user> user = nullptr;
    std::string user_id;

    sql::Session sql_sess = _server->get_sql_session();
    sql::Table users_table(_server->get_sql_schema(sql_sess), "users");
    sql::Row user_res;

    std::unique_lock lk(_sessions_mutex, std::defer_lock);

    // Try to get the user id for the session
    user_id = _server->session_manager()->get_user_id_for_session(session_id);

    try {
        // Search for the user row in the database
        user_res = users_table.select("*")
                       .where("id = :user_id")
                       .bind("user_id", user_id)
                       .execute()
                       .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the user is not found
    if (user_res.isNull()) {
        throw exception(error::user_not_found);
    }

    // Create the user from the db response
    user = std::make_shared<quesync::user>((std::string)user_res[0], (std::string)user_res[1],
                                           (std::string)user_res[3],
                                           ((std::string)user_res[4]).c_str(), user_res[5],
                                           get_friends(sql_sess, (std::string)user_res[0]),
                                           get_friend_requests(sql_sess, (std::string)user_res[0]));

    // Lock the mutex
    lk.lock();

    // Add the user to the authenticated sessions
    _authenticated_sessions.insert_or_assign((std::string)user_res[0], sess);

    return user;
}