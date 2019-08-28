#include "output.h"

#include <chrono>
#include <limits>
#include <sole.hpp>

#include "manager.h"

#include "../../../shared/packets/voice_participant_packet.h"
#include "../../../shared/utils/encryption.h"

quesync::client::voice::output::output(std::shared_ptr<manager> manager)
    : _manager(manager), _enabled(false), _deafen(false) {
    int opus_error = 0;

    // Create the opus decoder for the voice
    _opus_decoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, &opus_error);
    if (opus_error != OPUS_OK) {
        std::cout << "An error occurred in initializing Opus Decoder! Exiting.." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create the thread of the output and detach it
    _thread = std::thread(&output::output_thread, this);
    _thread.detach();
}

quesync::client::voice::output::~output() {
    // Destory opus decoder
    opus_decoder_destroy(_opus_decoder);
}

void quesync::client::voice::output::callback_handler(void *output_buffer) {
    std::unique_lock lk(_data_mutex);

    // If empty, clean output buffer
    if (_output_data.empty()) {
        memset(output_buffer, 0, FRAME_SIZE * PLAYBACK_CHANNELS * sizeof(int16_t));
        return;
    }

    // Pop the last output data
    std::shared_ptr<int16_t> buf = _output_data.front();
    _output_data.pop();

    // Unlock the mutex
    lk.unlock();

    // Copy the buffer to the output buffer + Convert from MONO to STEREO
    for (int i = 0; i < FRAME_SIZE; i++) {
        *((int16_t *)output_buffer + i * PLAYBACK_CHANNELS) =
            *((int16_t *)output_buffer + i * PLAYBACK_CHANNELS + 1) = buf.get()[i];
    }
}

void quesync::client::voice::output::enable() {
    std::queue<std::shared_ptr<int16_t>> q;

    // Swap with a clean queue to clean it
    std::swap(q, _output_data);

    _enabled = true;
}

void quesync::client::voice::output::disable() { _enabled = false; }

void quesync::client::voice::output::output_thread() {
    int decoded_size = 0, recv_bytes = 0;
    char recv_buffer[RECV_BUFFER_SIZE] = {0};

    int16_t pcm[FRAME_SIZE] = {0};

    udp::endpoint sender_endpoint;
    std::shared_ptr<packets::voice_participant_packet> voice_packet;

    while (true) {
        std::shared_ptr<int16_t> buffer(new int16_t[FRAME_SIZE]);

        // If disabled, skip
        if (!_enabled) {
            std::this_thread::sleep_for(std::chrono::microseconds(750));
            continue;
        }

        // Receive from the server the encoded voice sample into the recv buffer
        recv_bytes = (int)_manager->socket().receive_from(
            asio::buffer(recv_buffer, RECV_BUFFER_SIZE), sender_endpoint);

        // If the sender is the server endpoint handle the voice sample
        if (sender_endpoint == _manager->endpoint()) {
            // Decode the voice packet
            voice_packet =
                utils::encryption::decrypt_voice_packet<packets::voice_participant_packet>(
                    std::string(recv_buffer, recv_bytes), _manager->aes_key().get(),
                    _manager->hmac_key().get());
            if (!voice_packet) {
                continue;
            }

            // Activate the user's voice
            _manager->activate_voice(voice_packet->user_id());

            // If deafen, continue
            if (_deafen) {
                continue;
            }

            // Decode the current sample from the client
            decoded_size =
                opus_decode(_opus_decoder, (const unsigned char *)voice_packet->voice_data(),
                            voice_packet->voice_data_len(), (opus_int16 *)pcm, FRAME_SIZE, 0);

            // Copy the PCM to the buffer
            memcpy(buffer.get(), pcm, FRAME_SIZE * sizeof(int16_t));

            // Push the output buffer to the output data queue
            std::unique_lock lk(_data_mutex);
            _output_data.push(buffer);
        }
    }
}

void quesync::client::voice::output::deaf() { _deafen = true; }

void quesync::client::voice::output::undeaf() { _deafen = false; }

bool quesync::client::voice::output::deafen() { return _deafen; }