#include "input.h"

#include <chrono>
#include <limits>
#include <sole.hpp>

#include "manager.h"

#include "../../../shared/packets/voice_packet.h"
#include "../../../shared/utils/encryption.h"

#undef max  // Fix a conflict with windows.h's max macro

quesync::client::voice::input::input(std::shared_ptr<manager> manager)
    : _manager(manager), _enabled(false), _muted(false) {
    int opus_error = 0;

    // Create the opus encoder for the recording
    _opus_encoder =
        opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opus_error);
    if (opus_error != OPUS_OK) {
        std::cout << "An error occurred in initializing Opus Encoder! Exiting.." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Init RNNoise
    _rnnoise_state = rnnoise_create(NULL);

    // Create the thread of the input
    _thread = std::thread(&input::input_thread, this);
}

quesync::client::voice::input::~input() {
    // Wake the thread up
    _data_cv.notify_one();

    // If the input thread is still alive, join it
    if (_thread.joinable()) {
        _thread.join();
    }

    // Deallocate the RNNoise state
    rnnoise_destroy(_rnnoise_state);

    // Destory opus encoder
    opus_encoder_destroy(_opus_encoder);
}

void quesync::client::voice::input::callback_handler(void *input_buffer) {
    std::unique_lock lk(_data_mutex);
    std::shared_ptr<int16_t> input_data(new int16_t[FRAME_SIZE]);

    // Copy to input data
    memcpy(input_data.get(), input_buffer, FRAME_SIZE * sizeof(int16_t));

    // Add the input data
    _input_data.push(input_data);

    // Notify the handle thread
    lk.unlock();
    _data_cv.notify_one();
}

void quesync::client::voice::input::enable() {
    std::queue<std::shared_ptr<int16_t>> q;

    // Swap with a clean queue to clean it
    std::swap(q, _input_data);

    _enabled = true;
}

void quesync::client::voice::input::disable() { _enabled = false; }

void quesync::client::voice::input::input_thread() {
    int encodedDataLen = 0;
    int16_t encoded_buffer[FRAME_SIZE] = {0};

    float rnnoise_buffer[FRAME_SIZE] = {0};

    uint8_t current_db_sample = 0;
    int16_t db_check_samples[FRAME_SIZE * (CHECK_DB_TIMEOUT / 10)];
    double db = 0;
    uint64_t last_activated = _manager->get_ms();

    packets::voice_packet voice_packet;
    std::string voice_packet_encrypted;

    while (true) {
        std::shared_ptr<int16_t> buffer;

        // If all threads needs to be stopped
        if (_manager->stop_threads()) {
            break;
        }

        // If disabled, skip
        if (!_enabled) {
            std::this_thread::sleep_for(std::chrono::microseconds(750));
            continue;
        }

        // Wait for new input
        std::unique_lock lk(_data_mutex);
        _data_cv.wait(lk, [&, this] { return !!_manager->stop_threads() || !_input_data.empty(); });
        if (_manager->stop_threads()) {
            break;
        }

        // Get input data
        buffer = _input_data.front();
        _input_data.pop();

        // If muted, continue
        if (_muted) {
            continue;
        }

        // Copy all PCM 16-bit short to an array of floats
        for (int i = 0; i < FRAME_SIZE; i++) {
            rnnoise_buffer[i] = ((opus_int16 *)buffer.get())[i];
        }

        // Process frame using RNNoise to reduce background noise
        rnnoise_process_frame(_rnnoise_state, rnnoise_buffer, rnnoise_buffer);

        // Copy back the result data to the buffer
        for (int i = 0; i < FRAME_SIZE; i++) {
            ((opus_int16 *)buffer.get())[i] = (opus_int16)rnnoise_buffer[i];
        }

        // Copy the current sample to the buffer of db check samples
        for (int i = 0; i < FRAME_SIZE; i++) {
            db_check_samples[current_db_sample * FRAME_SIZE + i] = buffer.get()[i];
        }

        // Check if reached the amount of samples needed for db check
        current_db_sample++;
        if (current_db_sample >= (CHECK_DB_TIMEOUT / 10)) {
            current_db_sample = 0;

            // Calculate the db of the db check samples
            db = calc_db(calc_rms(db_check_samples, FRAME_SIZE * (CHECK_DB_TIMEOUT / 10)));
        }

        // If the current samples are over the minimum db or we are in the stop transition time
        if (db > MINIMUM_DB || _manager->get_ms() - last_activated < VOICE_DEACTIVATE_DELAY) {
            // Activate the user's voice
            _manager->activate_voice(_manager->user_id());

            // Encode the captured data
            encodedDataLen =
                opus_encode(_opus_encoder, (const opus_int16 *)buffer.get(), FRAME_SIZE,
                            (unsigned char *)encoded_buffer, FRAME_SIZE * sizeof(opus_int16));

            // Create the voice packet and encrypt it
            voice_packet = packets::voice_packet(_manager->user_id(), _manager->session_id(),
                                                 _manager->channel_id(), (char *)encoded_buffer,
                                                 encodedDataLen);
            voice_packet_encrypted = utils::encryption::encrypt_voice_packet(
                &voice_packet, _manager->aes_key().get(), _manager->hmac_key().get());

            try {
                // Send the encoded voice packet to the server
                _manager->socket().send_to(
                    asio::buffer(voice_packet_encrypted.c_str(), voice_packet_encrypted.length()),
                    _manager->endpoint());
            } catch (...) {
                break;
            }

            // If the current samples are over the minimum db, set the last played time
            if (db > MINIMUM_DB) {
                last_activated = _manager->get_ms();
            }
        }
    }
}

double quesync::client::voice::input::calc_rms(int16_t *data, uint32_t size) {
    double square_sum = 0, rms;
    float relative_value;

    const float max_value = std::numeric_limits<int16_t>::max();

    for (unsigned int i = 0; i < size; i++) {
        relative_value = data[i] / max_value;
        square_sum += relative_value * relative_value;
    }

    rms = sqrt(square_sum / size);
    return rms;
}

double quesync::client::voice::input::calc_db(double rms) { return 20 * log10(rms / AMP_REF); }

void quesync::client::voice::input::mute() { _muted = true; }

void quesync::client::voice::input::unmute() { _muted = false; }

bool quesync::client::voice::input::muted() { return _muted; }