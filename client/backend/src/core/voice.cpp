#include "voice.h"

#include "client.h"

#include "../../../../shared/packets/call_request_packet.h"
#include "../../../../shared/packets/join_call_request_packet.h"
#include "../../../../shared/packets/leave_call_packet.h"
#include "../../../../shared/packets/set_voice_state_packet.h"
#include "../../../../shared/utils/crypto/base64.h"
#include "../../../../shared/utils/memory.h"

quesync::client::modules::voice::voice(std::shared_ptr<quesync::client::client> client)
    : module(client), _voice_manager(nullptr) {}

std::shared_ptr<quesync::call_details> quesync::client::modules::voice::call(
    std::string channel_id) {
    packets::call_request_packet call_request_packet;

    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<unsigned char> aes_key, hmac_key;
    std::string otp;

    std::shared_ptr<call_details> call_details;

    // Check if voice initialized and connected
    if (!_voice_manager) {
        throw exception(error::no_connection);
    }

    // Init call packet
    call_request_packet =
        packets::call_request_packet(channel_id, _voice_manager->muted(), _voice_manager->deafen());

    // Send to the server the call request
    response_packet = _client->communicator()->send_and_verify(&call_request_packet,
                                                               packet_type::call_started_packet);

    // Decode AES Key, HMAC Key and the OTP
    aes_key = utils::memory::convert_to_buffer<unsigned char>(
        utils::crypto::base64::decode(response_packet->json()["voiceSessionAESKey"]));
    hmac_key = utils::memory::convert_to_buffer<unsigned char>(
        utils::crypto::base64::decode(response_packet->json()["voiceSessionHMACKey"]));
    otp = utils::crypto::base64::decode(response_packet->json()["voiceSessionOTP"]);

    // Enable the voice
    _voice_manager->enable(response_packet->json()["voiceSessionId"], channel_id, aes_key, hmac_key,
                           otp);

    // Parse the call details
    call_details = std::make_shared<quesync::call_details>(
        response_packet->json()["callDetails"].get<quesync::call_details>());

    return call_details;
}

std::unordered_map<std::string, quesync::voice::state> quesync::client::modules::voice::join_call(
    std::string channel_id) {
    packets::join_call_request_packet join_call_request_packet;

    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<unsigned char> aes_key, hmac_key;
    std::string otp;

    std::unordered_map<std::string, quesync::voice::state> voice_states;

    // Check if voice initialized and connected
    if (!_voice_manager) {
        throw exception(error::no_connection);
    }

    // Init join call packet
    join_call_request_packet = packets::join_call_request_packet(
        channel_id, _voice_manager->muted(), _voice_manager->deafen());

    // Send to the server the join call request
    response_packet = _client->communicator()->send_and_verify(
        &join_call_request_packet, packet_type::join_call_approved_packet);

    // Decode AES Key, HMAC Key and the OTP
    aes_key = utils::memory::convert_to_buffer<unsigned char>(
        utils::crypto::base64::decode(response_packet->json()["voiceSessionAESKey"]));
    hmac_key = utils::memory::convert_to_buffer<unsigned char>(
        utils::crypto::base64::decode(response_packet->json()["voiceSessionHMACKey"]));
    otp = utils::crypto::base64::decode(response_packet->json()["voiceSessionOTP"]);

    // Enable the voice
    _voice_manager->enable(response_packet->json()["voiceSessionId"], channel_id, aes_key, hmac_key,
                           otp);

    // Parse voice states
    voice_states = response_packet->json()["voiceStates"]
                       .get<std::unordered_map<std::string, quesync::voice::state>>();

    return voice_states;
}

void quesync::client::modules::voice::leave_call() {
    packets::leave_call_packet leave_call_packet;

    // Send the leave call request to the server
    std::shared_ptr<response_packet> response_packet =
        _client->communicator()->send_and_verify(&leave_call_packet, packet_type::call_left_packet);

    // Check if voice initialized and connected
    if (!_voice_manager) {
        throw exception(error::no_connection);
    }

    // Disable the voice
    _voice_manager->disable();
}

std::shared_ptr<quesync::voice::state> quesync::client::modules::voice::set_voice_state(
    bool mute, bool deafen) {
    packets::set_voice_state_packet set_voice_state_packet(mute, deafen);

    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<quesync::voice::state> voice_state;

    // Check if voice initialized and connected
    if (!_voice_manager) {
        throw exception(error::no_connection);
    }

    // Set the voice state
    _voice_manager->set_state(mute, deafen);

    // If the voice isn't connected
    if (!_voice_manager->enabled()) {
        // Create a voice state from the new state
        voice_state = std::make_shared<quesync::voice::state>(
            quesync::voice::state_type::disconnected, mute, deafen);
    } else {
        // Send the voice state request to the server
        response_packet = _client->communicator()->send_and_verify(
            &set_voice_state_packet, packet_type::voice_state_set_packet);

        // Create a voice  state from the new state
        voice_state = std::make_shared<quesync::voice::state>(quesync::voice::state_type::connected,
                                                              mute, deafen);
    }

    return voice_state;
}

void quesync::client::modules::voice::clean() {
    // Disable the voice if enabled
    if (_voice_manager) _voice_manager->disable();
}

void quesync::client::modules::voice::connected(std::string server_ip) {
    _voice_manager = std::make_shared<quesync::client::voice::manager>(_client, server_ip.c_str());
    _voice_manager->init();
}