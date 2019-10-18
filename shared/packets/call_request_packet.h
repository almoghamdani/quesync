#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../events/incoming_call_event.h"
#include "../exception.h"
#include "../utils/crypto/aes256.h"
#include "../utils/crypto/base64.h"
#include "../utils/crypto/hmac.h"
#include "../voice_state.h"

namespace quesync {
namespace packets {
class call_request_packet : public serialized_packet {
   public:
    call_request_packet() : call_request_packet("", false, false){};

    call_request_packet(std::string channel_id, bool muted, bool deafen)
        : serialized_packet(packet_type::call_request_packet) {
        _data["channelId"] = channel_id;
        _data["muted"] = muted;
        _data["deafen"] = deafen;
    };

    virtual bool verify() const {
        return exists("channelId") && exists("muted") && exists("deafen");
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<events::incoming_call_event> call_event;

        std::shared_ptr<call_details> call_details;
        std::pair<std::string, voice::encryption_info> voice_session_details;
        std::vector<std::string> users;

        nlohmann::json res;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Get the channel members
            users = session->server()->channel_manager()->get_channel_members(session->get_shared(),
                                                                              _data["channelId"]);
            users.push_back(session->user()->id);

            try {
                // Try to leave the current voice channel if in one
                session->server()->voice_manager()->leave_voice_channel(session->user()->id);
            } catch (...) {
            }

            // Create a voice session for the user
            voice_session_details =
                session->server()->voice_manager()->create_voice_session(session->user()->id);

            // Generate OTP for the session
            res["voiceSessionOTP"] = utils::crypto::base64::encode(
                session->server()->voice_manager()->generate_otp(voice_session_details.first));

            // Create a voice channel for the users
            call_details = session->server()->voice_manager()->init_voice_channel(
                session->user()->id, _data["channelId"], users);

            // Join the voice channel
            session->server()->voice_manager()->join_voice_channel(
                session->user()->id, _data["channelId"], _data["muted"], _data["deafen"]);

            // Create the call event
            call_event = std::make_shared<events::incoming_call_event>(call_details->call);

            // Update the voice states
            call_details->voice_states =
                session->server()->voice_manager()->get_voice_states(_data["channelId"]);

            // Remove the user from the voice states
            call_details->voice_states.erase(session->user()->id);

            // Set the user as joined to the call
            call_details->call.joined = true;

            // Set the call id, voice session details and the voice states
            res["voiceSessionAESKey"] = utils::crypto::base64::encode(
                std::string((char *)voice_session_details.second.aes_key.get(), AES_KEY_SIZE));
            res["voiceSessionHMACKey"] = utils::crypto::base64::encode(
                std::string((char *)voice_session_details.second.hmac_key.get(), HMAC_KEY_SIZE));
            res["voiceSessionId"] = voice_session_details.first;
            res["callDetails"] = *call_details;

            // Send the call event to the other users
            for (auto &user : users) {
                if (user != session->user()->id) {
                    session->server()->event_manager()->trigger_event(
                        std::static_pointer_cast<quesync::event>(call_event), user);
                }
            }

            // Return response packet with the voice info
            return response_packet(packet_type::call_started_packet, res).encode();
        } catch (exception &ex) {
            // Return the error code
            return error_packet(ex.error_code()).encode();
        } catch (...) {
            return error_packet(error::unknown_error).encode();
        }
    };
#endif
};
};  // namespace packets
};  // namespace quesync