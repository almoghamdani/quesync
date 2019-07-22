#pragma once

#include <sstream>
#include <string>

#include "packet.h"
#include "../utils.h"

#define MAX_VOICE_DATA_LEN 4096

class ParticipantVoicePacket
{
public:
	ParticipantVoicePacket(){};
	ParticipantVoicePacket(std::string user_id, char *voice_data, unsigned int voice_data_len)
		: _user_id(user_id), _voice_data(new char[voice_data_len]), _voice_data_len(voice_data_len)
	{
		memcpy(_voice_data, voice_data, voice_data_len);
	}

	std::string encode()
	{
		std::stringstream encoded_packet;

		encoded_packet << _user_id << PACKET_DELIMETER;
		encoded_packet << std::string(_voice_data, _voice_data_len) << PACKET_DELIMETER;

		return encoded_packet.str();
	}

	bool decode(std::string buf)
	{
		// Split the packet
		std::vector<std::string> params = Utils::Split(buf, PACKET_DELIMETER);

		if (params.size() >= 2)
		{
			// Parse the packet
			_user_id = params[0];
			
			// Parse voice data
			_voice_data_len = (unsigned int)(buf.length() - (_user_id.length() + 2));
			_voice_data = new char[_voice_data_len];
			memcpy(_voice_data, buf.c_str() + (_user_id.length() + 1), _voice_data_len);
		} else {
			return false;
		}

		return true;
	}

	std::string user_id()
	{
		return _user_id;
	}

	char *voice_data()
	{
		return _voice_data;
	}

	unsigned int voice_data_len()
	{
		return _voice_data_len;
	}

private:
	std::string _user_id;

	char *_voice_data;
	unsigned int _voice_data_len;
};