#include "channel_manager.h"

#include <ctime>
#include <sole.hpp>

#include "server.h"
#include "session.h"

#include "../shared/exception.h"

quesync::server::channel_manager::channel_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server),
      channels_table(server->db(), "channels"),
      channel_members_table(server->db(), "channel_members") {}

bool quesync::server::channel_manager::does_channel_exists(std::string channel_id) {
    try {
        return channels_table.select("1")
            .where("id = :channel_id")
            .bind("channel_id", channel_id)
            .execute()
            .count();
    } catch (...) {
        return false;
    }
}

std::shared_ptr<quesync::channel> quesync::server::channel_manager::get_private_channel(
    std::shared_ptr<quesync::server::session> sess, std::string user_id) {
    std::shared_ptr<channel> channel;
    sql::Row channel_res;

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // If the user is trying to get a private channel with itself, throw error
    if (sess->user()->id == user_id) {
        throw exception(error::self_private_channel);
    }

    // Check if the other user exists
    if (!_server->user_manager()->does_user_exists(user_id)) {
        throw exception(error::user_not_found);
    }

    // Try to get the private channel of the 2 users
    try {
        channel_res = _server->db()
                          .getSession()
                          .sql("CALL get_private_channel(?, ?);")
                          .bind(sess->user()->id)
                          .bind(user_id)
                          .execute()
                          .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the channel is null create the channel
    if (channel_res.isNull()) {
        // Create a channel for the users
        channel = create_channel(true);

        // Add the users as a members in the channel
        add_member_to_channel(channel->id, sess->user()->id);
        add_member_to_channel(channel->id, user_id);
    } else {
        // Get the channel
        channel = get_channel((std::string)channel_res[0]);
    }

    return channel;
}

bool quesync::server::channel_manager::is_user_member_of_channel(std::string user_id,
                                                                 std::string channel_id) {
    try {
        return channel_members_table.select("1")
            .where("channel_id = :channel_id AND member_id = :user_id")
            .bind("channel_id", channel_id)
            .bind("user_id", user_id)
            .execute()
            .count();
    } catch (...) {
        return false;
    }
}

void quesync::server::channel_manager::add_member_to_channel(std::string channel_id,
                                                             std::string member_id) {
    // If the channel is not found, throw error
    if (!does_channel_exists(channel_id)) {
        throw exception(error::channel_not_found);
    }

    // If the member already exists, throw error
    if (is_user_member_of_channel(member_id, channel_id)) {
        throw exception(error::already_member);
    }

    try {
        // Add the user as a member in the room
        channel_members_table.insert("channel_id", "member_id")
            .values(channel_id, member_id)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }
}

std::shared_ptr<quesync::channel> quesync::server::channel_manager::get_channel(
    std::string channel_id) {
    std::shared_ptr<channel> channel;
    sql::Row channel_res;

    // Try to get the private channel of the 2 users
    try {
        channel_res = channels_table.select("id", "is_private", "unix_timestamp(created_at)")
                          .where("id = :channel_id")
                          .bind("channel_id", channel_id)
                          .execute()
                          .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the channel res is null, return null
    if (channel_res.isNull()) {
        throw exception(error::channel_not_found);
    }

    // Create the channel object and return it
    channel = std::make_shared<quesync::channel>(
        (std::string)channel_res[0], (bool)channel_res[1], (int)channel_res[2],
        _server->voice_manager()->is_voice_channel_active(channel_id));
    return channel;
}

std::shared_ptr<quesync::channel> quesync::server::channel_manager::create_channel(
    bool is_private) {
    std::shared_ptr<channel> channel;
    std::string channel_id = sole::uuid4().str();

    try {
        // Try to add the new channel
        channels_table.insert("id", "is_private").values(channel_id, is_private).execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Create the channel object
    channel = std::make_shared<quesync::channel>(channel_id, is_private, std::time(nullptr), false);

    return channel;
}

std::vector<std::string> quesync::server::channel_manager::get_channel_members(
    std::shared_ptr<quesync::server::session> sess, std::string channel_id) {
    std::vector<std::string> members;

    sql::RowResult res;
    sql::Row row;

    // Check if the channel exists
    if (!does_channel_exists(channel_id)) {
        throw exception(error::channel_not_found);
    }

    // Check if the user is a member of the wanted channel
    if (!is_user_member_of_channel(sess->user()->id, channel_id)) {
        throw exception(error::not_member_of_channel);
    }

    try {
        // Get all members of channel except for the user itself
        res = channel_members_table.select("member_id")
                  .where("channel_id = :channel_id AND member_id != :user_id")
                  .bind("channel_id", channel_id)
                  .bind("user_id", sess->user()->id)
                  .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Add each member to the members vector
    while ((row = res.fetchOne())) {
        members.push_back((std::string)row[0]);
    }

    return members;
}
