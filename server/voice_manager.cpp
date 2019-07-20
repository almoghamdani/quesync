#include "voice_manager.h"

#include <sole.hpp>

#include "quesync.h"
#include "../shared/quesync_exception.h"

VoiceManager::VoiceManager(std::shared_ptr<Quesync> server) : Manager(server),
															  _socket(server->get_io_context(), udp::endpoint(udp::v4(), VOICE_SERVER_PORT))
{
	// Start receiving
	recv();
}

void VoiceManager::recv()
{
	_socket.async_receive_from(
		asio::buffer(_buf, MAX_DATA_LEN), _sender_endpoint,
		[this](std::error_code ec, std::size_t bytes) {
			if (!ec && bytes > 0)
			{
				send(bytes);
			}
			else
			{
				recv();
			}
		});
}

void VoiceManager::send(std::size_t length)
{
	_socket.async_send_to(
		asio::buffer(_buf, length), _sender_endpoint,
		[this](std::error_code, std::size_t) {
			recv();
		});
}

std::string VoiceManager::createVoiceSession(std::string user_id)
{
	std::string session_id = sole::uuid4().str();

	_sessions[user_id] = session_id;

	return session_id;
}

void VoiceManager::deleteVoiceSession(std::string user_id)
{
	try
	{
		_sessions.erase(user_id);
	}
	catch (...)
	{
		throw QuesyncException(VOICE_NOT_CONNECTED);
	}
}

std::string VoiceManager::createVoiceChannel(std::vector<std::string> users)
{
	std::string channel_id = sole::uuid4().str();

	_voice_channels[channel_id] = users;

	return channel_id;
}

void VoiceManager::closeVoiceChannel(std::string channel_id)
{
	try
	{
		_voice_channels.erase(channel_id);
	}
	catch (...)
	{
		throw QuesyncException(CHANNEL_NOT_FOUND);
	}
}