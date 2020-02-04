#pragma once
#include "manager.h"

#include <asio.hpp>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../shared/call_details.h"
#include "../../shared/event.h"
#include "../../shared/voice_state.h"

#define VOICE_SERVER_PORT 61111

#define MAX_DATA_LEN 8192

#define MAX_PENDING_SECONDS 20

#define MAX_CALLS_AMOUNT 250

using asio::ip::udp;

namespace quesync {
namespace voice {
struct encryption_info {
    /// The buffer containing the AES key.
    std::shared_ptr<unsigned char> aes_key;

    /// The buffer containing the HMAC key.
    std::shared_ptr<unsigned char> hmac_key;
};
};  // namespace voice

namespace server {
// Prevent loop header include
class session;

class voice_manager : manager {
   public:
    /**
     * Voice manager constructor.
     *
     * @param server A shared pointer to the server object.
     */
    voice_manager(std::shared_ptr<server> server);

    /**
     * Creates a voice session for a user.
     *
     * @param user_id The id of the user.
     * @return A pair with the session id and trhe encryption info of the voice stream.
     */
    std::pair<std::string, voice::encryption_info> create_voice_session(std::string user_id);

    /**
     * Delete user's voice session.
     *
     * @param user_id The id of the user.
     */
    void delete_voice_session(std::string user_id);

    /**
     * Generate OTP for a user's voice session.
     *
     * @param session_id The id of the session.
     * @return The OTP for the user's voice session.
     */
    std::string generate_otp(std::string session_id);

    /**
     * Initialize a voice channel.
     *
     * @param caller_id The id of the caller.
     * @param channel_id The id of the channel.
     * @param users The users in the channel.
     * @return A shared pointer to the call details object.
     */
    std::shared_ptr<call_details> init_voice_channel(std::string caller_id, std::string channel_id,
                                                     std::vector<std::string> users);

    /**
     * Checks if a voice channel is active.
     *
     * @param channel_id The id of the channel.
     * @return True if the voice channel is active or false otherwise.
     */
    bool is_voice_channel_active(std::string channel_id);

    /**
     * Joins user to a voice channel.
     *
     * @param user_id The id of the user.
     * @param channel_id The id of the channel.
     * @param muted The mute status of the input device for the user.
     * @param deafen The mute status of the output device for the user.
     */
    void join_voice_channel(std::string user_id, std::string channel_id, bool muted, bool deafen);

    /**
     * Removes user from voice channel.
     *
     * @param user_id The id of the user.
     */
    void leave_voice_channel(std::string user_id);

    /**
     * Sets the user's voice state.
     *
     * @param user_id The id of the user.
     * @param muted The mute status of the input device for the user.
     * @param deafen The mute status of the output device for the user.
     */
    void set_voice_state(std::string user_id, bool muted, bool deafen);

    /**
     * Get the voice states of a channel.
     *
     * @param channel_id The id of the channel.
     * @return A map containing the voice states.
     */
    std::unordered_map<std::string, voice::state> get_voice_states(std::string channel_id);

    /**
     * Get channel calls history.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param channel_id The id of the channel.
     * @param amount The amount of calls to request from the server.
     * @param offset The offset in the list of calls.
     * @return A vector containing the calls from the channel.
     */
    std::vector<call> get_channel_calls(std::shared_ptr<session> sess, std::string channel_id,
                                        int amount, int offset);

   private:
    udp::socket _socket;
    udp::endpoint _sender_endpoint;
    char _buf[MAX_DATA_LEN];

    std::unordered_map<std::string, std::shared_ptr<call_details>> _voice_channels;
    std::unordered_map<std::string, std::string> _joined_voice_channels;

    std::unordered_map<std::string, udp::endpoint> _session_endpoints;
    std::unordered_map<std::string, voice::encryption_info> _session_keys;
    std::unordered_map<std::string, std::string> _sessions;

    std::unordered_map<std::string, std::string> _otps;

    std::mutex _mutex;

    std::thread _voice_states_thread;

    void recv();
    void send(std::shared_ptr<char> buf, std::size_t length, udp::endpoint endpoint);

    void handle_packet(std::size_t length);

    void handle_voice_states();
    void trigger_voice_state_event(std::string channel_id, std::string user_id,
                                   voice::state voice_state);

    call create_call(std::string caller_id, std::string channel_id);
    void add_participant_to_call(std::string channel_id, std::string participant_id);
    void close_call(std::string channel_id);
    bool user_joined_call(std::string call_id, std::string user_id);
};
};  // namespace server
};  // namespace quesync
