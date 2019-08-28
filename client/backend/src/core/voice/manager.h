#pragma once

#include <RtAudio.h>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "../socket_manager.h"
#include "input.h"
#include "output.h"

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
    bool activated;
    uint64_t last_activated;
};

class manager : public std::enable_shared_from_this<manager> {
   public:
    manager(std::shared_ptr<client> client, const char *server_ip);

    void init();

    void enable(std::string session_id, std::string channel_id,
                std::shared_ptr<unsigned char> aes_key, std::shared_ptr<unsigned char> hmac_key,
                std::string otp);
    void disable();
    bool enabled();

    void set_state(bool mute, bool deafen);

    bool muted();
    bool deafen();

    void activate_voice(std::string user_id);

    std::shared_ptr<unsigned char> aes_key();
    std::shared_ptr<unsigned char> hmac_key();

    std::string user_id();
    std::string session_id();
    std::string channel_id();

    udp::socket &socket();
    udp::endpoint &endpoint();

    uint64_t get_ms();

    friend int rt_callback(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                          double streamTime, RtAudioStreamStatus status, void *userData);

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

    RtAudio _rt_audio;

    bool _enabled;

    void voice_activation_thread();

    void send_otp_packet(std::string otp);
};
};  // namespace voice
};  // namespace client
};  // namespace quesync