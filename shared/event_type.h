#pragma once

namespace quesync {
enum class event_type {
    ping_event,
    friend_request_event,
    friendship_status_event,
    message_event,
    incoming_call_event,
    voice_state_event,
    voice_activity_event,
    call_ended_event,
    file_transmission_progress_event
};
};
