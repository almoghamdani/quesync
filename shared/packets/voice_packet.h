#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "packet.h"
#include "../utils.h"

#define MAX_VOICE_DATA_LEN 4096

class VoicePacket
{
public:
	VoicePacket(){};
	VoicePacket(std::string user_id, std::string session_id, std::string channel_id, char *voice_data, unsigned int voice_data_len)
		: _user_id(user_id), _session_id(session_id), _channel_id(channel_id), _voice_data(new char[voice_data_len]), _voice_data_len(voice_data_len)
	{
		memcpy(_voice_data, voice_data, voice_data_len);
	}

	std::string encode()
	{
		std::stringstream encoded_packet;

		encoded_packet << _user_id << PACKET_DELIMETER;
		encoded_packet << _session_id << PACKET_DELIMETER;
		encoded_packet << _channel_id << PACKET_DELIMETER;
		encoded_packet << std::string(_voice_data, _voice_data_len) << PACKET_DELIMETER;

		return encoded_packet.str();
	}

	bool decode(std::string buf)
	{
		// Split the packet
		std::vector<std::string> params = Utils::Split(buf, PACKET_DELIMETER);

		if (params.size() >= 4)
		{
			// Parse the packet
			_user_id = params[0];
			_session_id = params[1];
			_channel_id = params[2];
			
			// Parse voice data
			_voice_data_len = buf.length() - (_user_id.length() + _session_id.length() + _channel_id.length() + 4);
			_voice_data = new char[_voice_data_len];
			memcpy(_voice_data, buf.c_str() + (_user_id.length() + _session_id.length() + _channel_id.length() + 3), _voice_data_len);
		} else {
			return false;
		}

		return true;
	}

	std::string user_id()
	{
		return _user_id;
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
	std::string _user_id;
	std::string _session_id;
	std::string _channel_id;

	char *_voice_data;
	unsigned int _voice_data_len;
};