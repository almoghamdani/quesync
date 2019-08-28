#include "channels.h"

#include "client.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/packets/get_private_channel_packet.h"

quesync::client::modules::channels::channels(std::shared_ptr<quesync::client::client> client)
    : module(client) {}

std::shared_ptr<quesync::channel> quesync::client::modules::channels::get_private_channel(
    std::string user_id) {
    packets::get_private_channel_packet get_private_channel_packet(user_id);

	// Send the get private channel request to the server
    std::shared_ptr<response_packet> response_packet = _client->communicator()->send_and_verify(
        &get_private_channel_packet, packet_type::private_channel_packet);

	// Parse the json to the channel object
	std::shared_ptr<channel> channel = std::make_shared<quesync::channel>();
	*channel = response_packet->json()["channel"];

	return channel;
}