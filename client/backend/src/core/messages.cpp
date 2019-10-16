#include "messages.h"

#include "client.h"

#include "../../../../shared/packets/get_channel_messages_packet.h"
#include "../../../../shared/packets/send_message_packet.h"

quesync::client::modules::messages::messages(std::shared_ptr<quesync::client::client> client)
    : module(client) {}

std::shared_ptr<quesync::message> quesync::client::modules::messages::send_message(
    std::string content, std::string channel_id, std::string attachment_id) {
    packets::send_message_packet send_message_packet(content, attachment_id, channel_id);

    // Send to the server the send message packet
    std::shared_ptr<response_packet> response_packet = _client->communicator()->send_and_verify(
        &send_message_packet, packet_type::message_id_packet);

    // Create the message object
    std::shared_ptr<message> message = std::make_shared<quesync::message>(
        response_packet->json()["messageId"], _client->auth()->get_user()->id, channel_id, content,
        attachment_id, std::time(nullptr));

    return message;
}

std::vector<quesync::message> quesync::client::modules::messages::get_channel_messages(
    std::string channel_id, unsigned int amount, unsigned int offset) {
    packets::get_channel_messages_packet get_channel_messages_packet(channel_id, amount, offset);

    // Send to the server the get messages packet
    std::shared_ptr<response_packet> response_packet = _client->communicator()->send_and_verify(
        &get_channel_messages_packet, packet_type::channel_messages_packet);

    // Parse the messages
    std::vector<quesync::message> messages =
        response_packet->json().get<std::vector<quesync::message>>();

    return messages;
}