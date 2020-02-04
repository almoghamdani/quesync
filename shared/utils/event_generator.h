#pragma once

#include <memory>
#include <unordered_map>

#include "../event.h"
#include "../event_type.h"

#include "../events/call_ended_event.h"
#include "../events/friend_request_event.h"
#include "../events/friendship_status_event.h"
#include "../events/incoming_call_event.h"
#include "../events/message_event.h"
#include "../events/voice_state_event.h"

#define EVENT_ENTRY(event_name) \
    { event_type::event_name, &event_generator::init_event<events::event_name> }

namespace quesync {
namespace utils {
class event_generator {
   public:
    /**
     * Generate an event object.
     *
     * @param type The type of the event.
     * @return A shared pointer to the new event object.
     */
    static std::shared_ptr<event> generate_event(event_type type) {
        try {
            return event_initalizers.at(type)();
        } catch (...) {
            return nullptr;
        }
    }

   private:
    static const std::unordered_map<event_type, std::function<std::shared_ptr<event>()>>
        event_initalizers;

    template <typename T>
    static std::shared_ptr<event> init_event() {
        return std::shared_ptr<event>(new T);
    }
};
};  // namespace utils
};  // namespace quesync