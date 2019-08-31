#include "message_manager.h"

#include <ctime>
#include <sole.hpp>

#include "event_manager.h"
#include "server.h"
#include "session.h"

#include "../shared/events/message_event.h"
#include "../shared/exception.h"

quesync::server::message_manager::message_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server), messages_table(server->db(), "messages") {}

std::shared_ptr<quesync::message> quesync::server::message_manager::send_message(
    std::shared_ptr<quesync::server::session> sess, std::string content, std::string channel_id) {
    std::shared_ptr<message> message;
    std::string message_id = sole::uuid4().str();
    std::vector<std::string> channel_members;

    std::shared_ptr<events::message_event> message_evt;

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // Check if the channel exists
    if (!_server->channel_manager()->does_channel_exists(channel_id)) {
        throw exception(error::channel_not_found);
    }

    // Check if the user is a member of the wanted channel
    if (!_server->channel_manager()->is_user_member_of_channel(sess->user()->id, channel_id)) {
        throw exception(error::not_member_of_channel);
    }

    try {
        // Try to insert the new message to the database
        messages_table.insert("id", "sender_id", "channel_id", "content")
            .values(message_id, sess->user()->id, channel_id, content)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Create the message object
    message = std::make_shared<quesync::message>(message_id, sess->user()->id, channel_id, content,
                                                 std::time(nullptr));

    // Create the event to be sent to the other online users
    message_evt = std::make_shared<events::message_event>(*message);

    // Get the other channel members
    channel_members = _server->channel_manager()->get_channel_members(sess, channel_id);

    // For each other channel member
    for (std::string &member_id : channel_members) {
        // Trigger the event in the other member
        _server->event_manager()->trigger_event(
            std::static_pointer_cast<quesync::event>(message_evt), member_id);
    }

    return message;
}

std::vector<quesync::message> quesync::server::message_manager::get_messages(
    std::shared_ptr<quesync::server::session> sess, std::string channel_id, unsigned int amount,
    unsigned int offset) {
    std::vector<message> messages;

    sql::RowResult res;
    sql::Row row;

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // Check if the channel exists
    if (!_server->channel_manager()->does_channel_exists(channel_id)) {
        throw exception(error::channel_not_found);
    }

    // Check if the user is a member of the wanted channel
    if (!_server->channel_manager()->is_user_member_of_channel(sess->user()->id, channel_id)) {
        throw exception(error::not_member_of_channel);
    }

    // Check if amount surpasses the limit
    if (amount > MAX_MESSAGES_AMOUNT) {
        throw exception(error::amount_exceeded_max);
    }

    try {
        // Try to get the messages from the table using the amount and offset
        res = messages_table
                  .select("id", "sender_id", "channel_id", "content", "unix_timestamp(sent_at)")
                  .where("channel_id = :channel_id")
                  .orderBy("sent_at DESC")
                  .limit(amount)
                  .offset(offset)
                  .bind("channel_id", channel_id)
                  .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // For each row in the result, create a message
    while ((row = res.fetchOne())) {
        // Create the message from the row
        messages.push_back(message((std::string)row[0], (std::string)row[1], (std::string)row[2],
                                   (std::string)row[3], (int)row[4]));
    }

    return messages;
}
