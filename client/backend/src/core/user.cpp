#include "user.h"

#include "client.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/packets/set_profile_photo_packet.h"

quesync::client::modules::user::user(std::shared_ptr<quesync::client::client> client)
    : module(client) {}

void quesync::client::modules::user::set_profile_photo(std::string file_id)
{
	packets::set_profile_photo_packet set_profile_photo_packet(file_id);

    // Send the set profile photo packet to the server
    _client->communicator()->send_and_verify(
        &set_profile_photo_packet, packet_type::profile_photo_set_packet);
}