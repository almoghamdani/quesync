#pragma once
#include "manager.h"

#include "../../shared/event.h"

namespace quesync {
namespace server {
class event_manager : manager {
   public:
    /**
     * Event maanger constructor.
     *
     * @param server A shared pointer to the server object.
     */
    event_manager(std::shared_ptr<server> server);

    /**
     * Trigger an event in a client.
     *
     * @param evt A shared pointer to the event object.
     * @param target_user_id The user id of the target client.
     */
    void trigger_event(std::shared_ptr<event> evt, std::string target_user_id);
};
};  // namespace server
};  // namespace quesync
