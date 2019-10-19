#pragma once
#include <string>
#include <unordered_map>

#include "../../../../shared/event_type.h"

namespace quesync {
namespace client {
namespace wrapper {
const std::unordered_map<event_type, std::string> event_names{
    {event_type::ping_event, "ping"},
    {event_type::friend_request_event, "friend-request"},
    {event_type::friendship_status_event, "friendship-status"},
    {event_type::message_event, "message"},
    {event_type::incoming_call_event, "incoming-call"},
    {event_type::voice_state_event, "voice-state"},
    {event_type::voice_activity_event, "voice-activity"},
    {event_type::call_ended_event, "call-ended"},
    {event_type::file_transmission_progress_event, "file-transmission-progress"},
    {event_type::server_disconnect_event, "server-disconnect"}};
};
};  // namespace client
};  // namespace quesync
