#pragma once
#include "serialized_object.h"

#include <ctime>

class Message : public SerializedObject
{
public:
	Message() : Message("", "", "", "", (std::time_t)0){};

	Message(std::string id, std::string sender_id,
			std::string channel_id,
			std::string content,
			std::time_t sent_at)
	{
		// Save all fields in the json type
		_json["id"] = id;
		_json["senderId"] = sender_id;
		_json["channelId"] = channel_id;
		_json["content"] = content;
		_json["sentAt"] = sent_at;
	};

	GET_FUNCTION(id, std::string)
	GET_FUNCTION(senderId, std::string)
	GET_FUNCTION(channelId, std::string)
	GET_FUNCTION(content, std::string)
	GET_FUNCTION(sentAt, std::time_t)
};