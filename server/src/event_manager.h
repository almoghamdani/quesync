#pragma once
#include "manager.h"

#include "../../shared/event.h"

namespace quesync {
namespace server {
class event_manager : manager {
   public:
    event_manager(std::shared_ptr<server> server);

    void trigger_event(std::shared_ptr<event> evt, std::string target_user_id);
};
};  // namespace server
};  // namespace quesync
