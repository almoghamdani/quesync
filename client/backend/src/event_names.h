#pragma once
#include <unordered_map>
#include <string>

#include "../../../shared/event_types.h"

const std::unordered_map<QuesyncEvent, std::string> EVENT_NAMES{
	{FRIEND_REQUEST_EVENT, "friend-request"},
	{MESSAGE_EVENT, "message"}};