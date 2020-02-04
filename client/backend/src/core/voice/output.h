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
    /**
     * Voice output constructer.
     *
     * @param manager Shared pointer to the voice manager.
     */
    output(std::shared_ptr<manager> manager);

    /**
     * Voice output destructor.
     */
    ~output();

    /**
     * Enable the voice output.
     */
    void enable();

    /**
     * Disable the voice output.
     */
    void disable();

    /**
     * Mute the voice output.
     */
    void deaf();

    /**
     * Unmute the voice output.
     */
    void undeaf();

    /**
     * Returns the mute status of the voice output.
     *
     * @return True if the voice output is muted or false otherwise.
     */
    bool deafen();

    /**
     * Handles the callback from the audio framework.
     *
     * @param output_buffer The buffer containing the output data.
     */
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