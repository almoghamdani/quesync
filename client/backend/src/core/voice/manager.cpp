#include "manager.h"

#include <chrono>
#include <iostream>
#include <sole.hpp>

#include "../client.h"

#include "../../../shared/events/voice_activity_event.h"
#include "../../../shared/packets/voice_otp_packet.h"
#include "../../../shared/packets/voice_packet.h"

int quesync::client::voice::rt_callback(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                                        double streamTime, RtAudioStreamStatus status,
                                        void *userData) {
    quesync::client::voice::manager *vm = (quesync::client::voice::manager *)userData;

    // Validate frame size
    if (nFrames != FRAME_SIZE) return 0;

    // Handle input and output
    vm->_input->callback_handler(inputBuffer);
    vm->_output->callback_handler(outputBuffer);

    return 0;
}

quesync::client::voice::manager::manager(std::shared_ptr<quesync::client::client> client,
                                         const char *server_ip)
    : _client(client),
      _socket(socket_manager::io_context,
              udp::endpoint(udp::v4(), 0)),  // Create an IPv4 UDP socket with a random port
      _aes_key(nullptr),
      _hmac_key(nullptr),
      _enabled(false) {
    // Get the endpoint of the server using the given server IP and default voice chat port
    socket_manager::get_endpoint(server_ip, VOICE_CHAT_PORT, _endpoint);

    // Create the thread of activating and deactivating voice
    _activation_thread = std::thread(&manager::voice_activation_thread, this);
    _activation_thread.detach();
}

void quesync::client::voice::manager::init() {
    unsigned int frame_size = FRAME_SIZE;

    // Set the input stream parameters
    RtAudio::StreamParameters input_stream_params;
    input_stream_params.deviceId = _rt_audio.getDefaultInputDevice();
    input_stream_params.nChannels = RECORD_CHANNELS;

    // Set the output stream parameters
    RtAudio::StreamParameters output_stream_params;
    output_stream_params.deviceId = _rt_audio.getDefaultOutputDevice();
    output_stream_params.nChannels = PLAYBACK_CHANNELS;

    RtAudio::StreamOptions stream_options;
    stream_options.streamName = "Quesync-" + sole::uuid4().str();

    // Open the stream
    _rt_audio.openStream(&output_stream_params, &input_stream_params, RTAUDIO_SINT16,
                         RECORD_FREQUENCY, &frame_size, &rt_callback, this, &stream_options);

    // Init input and output
    _input = std::make_shared<input>(shared_from_this());
    _output = std::make_shared<output>(shared_from_this());
}

void quesync::client::voice::manager::enable(std::string session_id, std::string channel_id,
                                             std::shared_ptr<unsigned char> aes_key,
                                             std::shared_ptr<unsigned char> hmac_key,
                                             std::string otp) {
    // Flush socket buffer
    size_t available = _socket.available();
    if (available) {
        _socket.receive_from(asio::buffer(std::shared_ptr<char>(new char[available]).get(), available), endpoint());
    }

    // If not enabled, send the OTP packet to the server to authenticate
    if (!_enabled) send_otp_packet(otp);

    _session_id = session_id;
    _channel_id = channel_id;
    _aes_key = aes_key;
    _hmac_key = hmac_key;

    // Start the stream
    _rt_audio.startStream();

    // Enable voice chat and input and output
    _enabled = true;
    _input->enable();
    _output->enable();
}

void quesync::client::voice::manager::disable() {
    // Disable voice chat and input and output
    _enabled = false;
    _input->disable();
    _output->disable();

    // Stop the stream
    try {
        _rt_audio.stopStream();
    } catch (...) {
    }
}

bool quesync::client::voice::manager::enabled() { return _enabled; }

void quesync::client::voice::manager::set_state(bool mute, bool deafen) {
    if (mute) {
        _input->mute();
    } else {
        _input->unmute();
    }

    if (deafen) {
        _output->deaf();
    } else {
        _output->undeaf();
    }
}

bool quesync::client::voice::manager::muted() { return _input->muted(); }

bool quesync::client::voice::manager::deafen() { return _output->deafen(); }

std::string quesync::client::voice::manager::user_id() { return _client->auth()->get_user()->id; }

std::string quesync::client::voice::manager::session_id() { return _session_id; }

std::string quesync::client::voice::manager::channel_id() { return _channel_id; }

udp::socket &quesync::client::voice::manager::socket() { return _socket; }

udp::endpoint &quesync::client::voice::manager::endpoint() { return _endpoint; }

void quesync::client::voice::manager::voice_activation_thread() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (!_enabled) continue;

        std::unique_lock lk(_activation_mutex);
        uint64_t current = get_ms();

        // For each user in the voice activation map
        for (auto &user : _voice_activation) {
            // If the user hasn't activated in the timeout
            if (user.second.activated &&
                current - user.second.last_activated >= DEACTIVIATION_TIMEOUT_MS) {
                user.second.activated = false;

                _changed_voice_activity[user.first] = false;
            }
        }

        // If user's activity has changed, call the voice activity event
        if (_changed_voice_activity.size()) {
            try {
                _client->communicator()->event_handler().call_event(std::static_pointer_cast<event>(
                    std::make_shared<events::voice_activity_event>(_changed_voice_activity)));
            } catch (...) {
            }
        }

        // Clear the changed voice activity
        _changed_voice_activity.clear();
    }
}

void quesync::client::voice::manager::activate_voice(std::string user_id) {
    std::unique_lock lk(_activation_mutex);

    // If the user already exists
    if (_voice_activation.count(user_id)) {
        // If not activated, active
        if (!_voice_activation[user_id].activated) {
            _voice_activation[user_id] = activation{true, get_ms()};
            _changed_voice_activity[user_id] = true;
        } else {
            _voice_activation[user_id].last_activated = get_ms();
        }
    } else {
        _voice_activation[user_id] = activation{true, get_ms()};
        _changed_voice_activity[user_id] = true;
    }
}

void quesync::client::voice::manager::send_otp_packet(std::string otp) {
    packets::voice_otp_packet otp_packet(otp);

    std::string otp_packet_encoded = otp_packet.encode();

    // Send the OTP packet to the server
    _socket.send_to(asio::buffer(otp_packet_encoded.c_str(), otp_packet_encoded.length()),
                    _endpoint);
}

std::shared_ptr<unsigned char> quesync::client::voice::manager::aes_key() { return _aes_key; }

std::shared_ptr<unsigned char> quesync::client::voice::manager::hmac_key() { return _hmac_key; }

uint64_t quesync::client::voice::manager::get_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
