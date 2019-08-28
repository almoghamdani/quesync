#include "users.h"

#include "client.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/packets/friend_request_packet.h"
#include "../../../../shared/packets/friendship_status_packet.h"
#include "../../../../shared/packets/profile_request_packet.h"
#include "../../../../shared/packets/search_packet.h"

quesync::client::modules::users::users(std::shared_ptr<quesync::client::client> client)
    : module(client) {}

std::shared_ptr<quesync::profile> quesync::client::modules::users::get_user_profile(
    std::string user_id) {
    packets::profile_request_packet profile_request_packet(user_id);

    // Send the profile request packet to the server and get resposne
    std::shared_ptr<response_packet> res_packet = _client->communicator()->send_and_verify(
        &profile_request_packet, packet_type::profile_packet);

    // Parse the profile from the json
    std::shared_ptr<profile> profile = std::make_shared<quesync::profile>();
    *profile = res_packet->json();

    return profile;
}

std::vector<quesync::profile> quesync::client::modules::users::search(std::string nickname,
                                                                      int tag) {
    packets::search_packet search_packet(nickname, tag);

    // Send the search request to the server
    std::shared_ptr<response_packet> res_packet = _client->communicator()->send_and_verify(
        &search_packet, packet_type::search_results_packet);

    // Return the search results
    return res_packet->json();
}

std::shared_ptr<quesync::friend_request> quesync::client::modules::users::send_friend_request(
    std::string friend_id) {
    packets::friend_request_packet friend_request_packet(friend_id);

    // Send the friend request to the server
    std::shared_ptr<response_packet> res_packet = _client->communicator()->send_and_verify(
        &friend_request_packet, packet_type::friend_request_sent_packet);

    // Create a friend request from the friend details
    return std::make_shared<friend_request>(friend_id, "recipient", std::time(nullptr));
}

void quesync::client::modules::users::set_friendship_status(std::string friend_id, bool status) {
    packets::friendship_status_packet friendship_status_packet(friend_id, status);

    // Send to the server the friendship status request
    _client->communicator()->send_and_verify(&friendship_status_packet,
                                             packet_type::friendship_status_set_packet);
}