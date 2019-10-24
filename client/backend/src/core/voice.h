#pragma once
#include "module.h"

#include <string>
#include <unordered_map>

#include "voice/manager.h"

#include "../../../../shared/call.h"
#include "../../../../shared/call_details.h"
#include "../../../../shared/voice_state.h"

namespace quesync {
namespace client {
namespace modules {
class voice : public module {
   public:
    voice(std::shared_ptr<client> client);

    std::shared_ptr<call_details> call(std::string channel_id);
    std::unordered_map<std::string, quesync::voice::state> join_call(std::string channel_id);
    std::shared_ptr<quesync::voice::state> set_voice_state(bool mute, bool deafen);
    std::vector<quesync::call> get_channel_calls(std::string channel_id, unsigned int amount,
                                        unsigned int offset);
    void leave_call();

    virtual void clean_connection();
    virtual void logged_out();
    virtual void connected(std::string server_ip);

   private:
    std::shared_ptr<quesync::client::voice::manager> _voice_manager;
};
};  // namespace modules
};  // namespace client
};  // namespace quesync