#pragma once
#include <RtAudio.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "../socket_manager.h"
#include "input.h"
#include "output.h"
#include "sound_device.h"

#define VOICE_CHAT_PORT 61111

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 1
#define PLAYBACK_CHANNELS 2
#define FRAME_SIZE 480

#define DEACTIVIATION_TIMEOUT_MS 250

namespace quesync {
namespace client {
class client;

namespace voice {
struct activation {
    /// Is the user's voice is activated.
    bool activated;

    /// Last time the voice was activated in unix timestamp.
    uint64_t last_activated;
};

class manager : public std::enable_shared_from_this<manager> {
   public:
    /**
     * Voice manager constructor.
     *
     * @param client Shared pointer to the client object.
     * @param server_ip The server IP.
     */
    manager(std::shared_ptr<client> client, const char *server_ip);

    /**
     * Initializes the voice manager.
     */
    void init();

    /**
     * Destroys the voice manager.
     */
    void destroy();

    /**
     * Creates a list of input devices.
     *
     * @return Vector containing the available input devices.
     */
    std::vector<sound_device> get_input_devices();

    /**
     * Creates a list of output devices.
     *
     * @return Vector containing the available output devices.
     */
    std::vector<sound_device> get_output_devices();

    /**
     * Sets the active input device.
     *
     * @param device_id The device id of the input device.
     */
    void set_input_device(unsigned int device_id);

    /**
     * Sets the active output device.
     *
     * @param device_id The device id of the output device.
     */
    void set_output_device(unsigned int device_id);

    /**
     * Enables the voice manager and starts the voice stream.
     *
     * @param session_id The voice session id.
     * @param channel_id The voice channel id.
     * @param aes_key A buffer containing the AES key for the voice stream.
     * @param hmac_key A buffer containing the HMAC key for the voice stream.
     * @param otp The one-time password for the voice stream.
     */
    void enable(std::string session_id, std::string channel_id,
                std::shared_ptr<unsigned char> aes_key, std::shared_ptr<unsigned char> hmac_key,
                std::string otp);

    /**
     * Disables the voice manager and stops the voice stream.
     */
    void disable();

    /**
     * Checks if the stream is enabled.
     *
     * @returns Is the voice stream enabled.
     */
    bool enabled();

    /**
     * Sets the mute and deafen state of the voice stream.
     *
     * @param mute Should the voice stream be muted.
     * @param deafen Should the voice stream be deafen.
     */
    void set_state(bool mute, bool deafen);

    /**
     * Checks if the voice stream is muted.
     *
     * @returns Is the voice stream muted.
     */
    bool muted();

    /**
     * Checks if the voice stream is deafen.
     *
     * @returns Is the voice stream deafen.
     */
    bool deafen();

    /**
     * Set the user's voice as active.
     *
     * @param user_id The id of the user to activate it's voice.
     */
    void activate_voice(std::string user_id);

    /**
     * Gets the voice stream AES key.
     *
     * @return A buffer containing the AES key.
     */
    std::shared_ptr<unsigned char> aes_key();

    /**
     * Gets the voice stream HMAC key.
     *
     * @return A buffer containing the HMAC key.
     */
    std::shared_ptr<unsigned char> hmac_key();

    /**
     * Gets the user id.
     *
     * @return The user id.
     */
    std::string user_id();

    /**
     * Gets the voice session id.
     *
     * @return The voice session id.
     */
    std::string session_id();

    /**
     * Gets the voice channel id.
     *
     * @return The channel id.
     */
    std::string channel_id();

    /**
     * Gets the voice stream socket.
     *
     * @return A reference to the voice stream socket.
     */
    udp::socket &socket();

    /**
     * Gets the voice server endpoint.
     *
     * @return A reference to the voice server's endpoint.
     */
    udp::endpoint &endpoint();

    /**
     * Gets the current timestamp in milliseconds.
     *
     * @return The current timestamp in milliseconds.
     */
    uint64_t get_ms();

    /**
     * Checks if the voice threads should be stopped.
     *
     * @return An atomic variable containing a bool specifing if the threads should be stopped.
     */
    std::atomic<bool> &stop_threads();

   private:
    std::shared_ptr<client> _client;

    std::shared_ptr<quesync::client::voice::input> _input;
    std::shared_ptr<quesync::client::voice::output> _output;

    std::shared_ptr<unsigned char> _aes_key;
    std::shared_ptr<unsigned char> _hmac_key;

    std::string _session_id;
    std::string _channel_id;

    udp::socket _socket;
    udp::endpoint _endpoint;

    std::mutex _activation_mutex;
    std::unordered_map<std::string, activation> _voice_activation;
    std::unordered_map<std::string, bool> _changed_voice_activity;

    std::thread _activation_thread;

    std::atomic<bool> _stop_threads;

    RtAudio _rt_audio;

    unsigned int _input_device_id;
    unsigned int _output_device_id;

    bool _enabled;

    void init_stream();

    void voice_activation_thread();

    void send_otp_packet(std::string otp);

    friend int rt_callback(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                           double streamTime, RtAudioStreamStatus status, void *userData);
};
};  // namespace voice
};  // namespace client
};  // namespace quesync