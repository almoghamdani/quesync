#include "event_generator.h"

const std::unordered_map<quesync::event_type, std::function<std::shared_ptr<quesync::event>()>>
    quesync::utils::event_generator::event_initalizers{
        EVENT_ENTRY(call_ended_event),        EVENT_ENTRY(incoming_call_event),
        EVENT_ENTRY(friend_request_event),    EVENT_ENTRY(message_event),
        EVENT_ENTRY(friendship_status_event), EVENT_ENTRY(voice_state_event)};