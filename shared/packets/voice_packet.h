#pragma once

#include <sstream>
#include <string>

#include "packet.h"
#include "../utils.h"

#define MAX_VOICE_DATA_LEN 4096

class VoicePacket
{
public:
	VoicePacket(){};
	VoicePacket(std::string session_id, std::string channel_id, unsigned char *voice_data, unsigned int voice_data_len)
		: _session_id(session_id), _channel_id(channel_id), _voice_data(new char[voice_data_len]), _voice_data_len(voice_data_len)
	{
		memcpy(_voice_data, voice_data, voice_data_len);
	}

	std::string encode()
	{
		std::stringstream encoded_packet;

		encoded_packet << _session_id << PACKET_DELIMETER;
		encoded_packet << _channel_id << PACKET_DELIMETER;
		encoded_packet << std::string(_voice_data, _voice_data_len) << PACKET_DELIMETER;

		return encoded_packet.str();
	}

	bool decode(std::string buf)
	{
		// Split the packet
		std::vector<std::string> params = Utils::Split(buf, PACKET_DELIMETER);

		if (params.size() == 3)
		{
			// Parse the packet
			_session_id = params[0];
			_channel_id = params[1];
			
			// Parse voice data
			_voice_data_len = params[2].length();
			_voice_data = new char[_voice_data_len];
			memcpy(_voice_data, params[2].c_str(), _voice_data_len);
		} else {
			return false;
		}

		return true;
	}

	std::string session_id()
	{
		return _session_id;
	}

	std::string channel_id()
	{
		return _channel_id;
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
	std::string _session_id;
	std::string _channel_id;

	char *_voice_data;
	unsigned int _voice_data_len;
};