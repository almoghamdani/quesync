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
    input(std::shared_ptr<manager> manager);
    ~input();

    void enable();
    void disable();

    void mute();
    void unmute();

    bool muted();

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
