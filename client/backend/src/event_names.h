#pragma once
#include <unordered_map>
#include <string>

#include "../../../shared/event_types.h"

const std::unordered_map<QuesyncEvent, std::string> EVENT_NAMES{
	{FRIEND_REQUEST_EVENT, "friend-request"},
	{FRIENDSHIP_STATUS_EVENT, "friendship-status"},
	{MESSAGE_EVENT, "message"},
	{INCOMING_CALL_EVENT, "incoming-call"},
	{VOICE_STATE_EVENT, "voice-state"}};