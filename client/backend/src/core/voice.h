#pragma once
#include "module.h"

#include <string>
#include <unordered_map>

#include "../../../../shared/call.h"
#include "../../../../shared/call_details.h"
#include "../../../../shared/voice_state.h"
#include "voice/manager.h"

namespace quesync {
namespace client {
namespace modules {
class voice : public module {
   public:
    /**
     * Voice module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    voice(std::shared_ptr<client> client);

    /**
     * Start a call in a channel.
     *
     * @param channel_id The id of the channel to start a call in.
     * @return A shared pointer to the call details object.
     */
    std::shared_ptr<call_details> call(std::string channel_id);

    /**
     * Join an existing call in a channel.
     *
     * @param channel_id The id of the channel to join to call in.
     * @return A map containing the current voice states in the call.
     */
    std::unordered_map<std::string, quesync::voice::state> join_call(std::string channel_id);

    /**
     * Sets the mute and deafen state of the voice stream.
     *
     * @param mute Should the voice stream be muted.
     * @param deafen Should the voice stream be deafen.
     * @return A shared pointer to the new voice state object.
     */
    std::shared_ptr<quesync::voice::state> set_voice_state(bool mute, bool deafen);

    /**
     * Gets channel recent calls.
     *
     * @param channel_id The id of the channel.
     * @param amount The amount of calls to request from the server.
     * @param offset The offset in the list of calls.
     * @return A vector containing the calls from the channel.
     */
    std::vector<quesync::call> get_channel_calls(std::string channel_id, unsigned int amount,
                                                 unsigned int offset);

    /**
     * Leave the current call the client is participating.
     */
    void leave_call();

    /**
     * Gets the list of input devices.
     *
     * @return Vector containing the available input devices.
     */
    std::vector<quesync::client::voice::sound_device> get_input_devices();

    /**
     * Gets the list of output devices.
     *
     * @return Vector containing the available output devices.
     */
    std::vector<quesync::client::voice::sound_device> get_output_devices();

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

    virtual void clean_connection();
    virtual void logged_out();
    virtual void connected(std::string server_ip);

   private:
    /// A shared pointer to the voice manager object.
    std::shared_ptr<quesync::client::voice::manager> _voice_manager;
};
};  // namespace modules
};  // namespace client
};  // namespace quesync