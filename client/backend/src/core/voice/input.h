#pragma once
#include <opus.h>
#include <rnnoise.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#define AMP_REF 0.00001
#define CHECK_DB_TIMEOUT 30
#define MINIMUM_DB 40
#define VOICE_DEACTIVATE_DELAY 100

namespace quesync {
namespace client {
namespace voice {
class manager;

class input {
   public:
    /**
     * Voice input constructer.
     *
     * @param manager Shared pointer to the voice manager.
     */
    input(std::shared_ptr<manager> manager);

    /**
     * Voice input destructor.
     */
    ~input();

    /**
     * Enable the voice input.
     */
    void enable();

    /**
     * Disable the voice input.
     */
    void disable();

    /**
     * Mute the voice input.
     */
    void mute();

    /**
     * Unmute the voice input.
     */
    void unmute();

    /**
     * Returns the mute status of the voice input.
     *
     * @return True if the voice input is muted or false otherwise.
     */
    bool muted();

    /**
     * Handles the callback from the audio framework.
     *
     * @param input_buffer The buffer containing the input data.
     */
    void callback_handler(void *input_buffer);

   private:
    std::shared_ptr<manager> _manager;

    std::queue<std::shared_ptr<int16_t>> _input_data;
    std::mutex _data_mutex;
    std::condition_variable _data_cv;

    std::thread _thread;

    OpusEncoder *_opus_encoder;
    DenoiseState *_rnnoise_state;

    bool _enabled;
    bool _muted;

    void input_thread();

    double calc_rms(int16_t *data, uint32_t size);
    double calc_db(double rms);
};
};  // namespace voice
};  // namespace client
};  // namespace quesync
