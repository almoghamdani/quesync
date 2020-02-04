#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../utils/crypto/aes256.h"
#include "../utils/crypto/base64.h"
#include "../voice_state.h"

namespace quesync {
namespace packets {
class join_call_request_packet : public serialized_packet {
   public:
    /// Default constructor.
    join_call_request_packet() : join_call_request_packet("", false, false){};

    /**
     * Packet constructor.
     *
     * @param channel_id The id of the channel.
     * @param muted The mute status of the input device.
     * @param deafen The mute status of the output device.
     */
    join_call_request_packet(std::string channel_id, bool muted, bool deafen)
        : serialized_packet(packet_type::join_call_request_packet) {
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
        std::pair<std::string, voice::encryption_info> voice_session_details;
        std::unordered_map<std::string, voice::state> voice_states;

        nlohmann::json res;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
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

            // Try to join the voice channel
            session->server()->voice_manager()->join_voice_channel(
                session->user()->id, _data["channelId"], _data["muted"], _data["deafen"]);

            // Get the voice states of the channel
            voice_states = session->server()->voice_manager()->get_voice_states(_data["channelId"]);

            // Remove the user from the voice states
            voice_states.erase(session->user()->id);

            // Set the voice session details and the voice states
            res["voiceSessionAESKey"] = utils::crypto::base64::encode(
                std::string((char *)voice_session_details.second.aes_key.get(), AES_KEY_SIZE));
            res["voiceSessionHMACKey"] = utils::crypto::base64::encode(
                std::string((char *)voice_session_details.second.hmac_key.get(), HMAC_KEY_SIZE));
            res["voiceSessionId"] = voice_session_details.first;
            res["voiceStates"] = voice_states;

            // Return response packet with the voice info
            return response_packet(packet_type::join_call_approved_packet, res).encode();
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