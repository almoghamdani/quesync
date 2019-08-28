#pragma once

#include <RtAudio.h>
#include <opus.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#define RECV_BUFFER_SIZE 8192

namespace quesync {
namespace client {
namespace voice {
class manager;

class output {
   public:
    output(std::shared_ptr<manager> manager);
    ~output();

    void enable();
    void disable();

    void deaf();
    void undeaf();

    bool deafen();

    void callback_handler(void *output_buffer);

   private:
    std::shared_ptr<manager> _manager;

    std::queue<std::shared_ptr<int16_t>> _output_data;
    std::mutex _data_mutex;

    std::thread _thread;

    OpusDecoder *_opus_decoder;

    bool _enabled;
    bool _deafen;

    void output_thread();
};
};  // namespace voice
};  // namespace client
};  // namespace quesync