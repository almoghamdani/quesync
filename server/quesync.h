#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <map>
#include <memory>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "user_manager.h"
#include "event_manager.h"
#include "channel_manager.h"
#include "message_manager.h"
#include "session_manager.h"
#include "voice_manager.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync : public std::enable_shared_from_this<Quesync>
{
public:
	Quesync(asio::io_context &io_context);
	~Quesync();

	void start();

	asio::io_context &get_io_context();

	std::shared_ptr<UserManager> userManager();
	std::shared_ptr<EventManager> eventManager();
	std::shared_ptr<ChannelManager> channelManager();
	std::shared_ptr<MessageManager> messageManager();
	std::shared_ptr<SessionManager> sessionManager();
	std::shared_ptr<VoiceManager> voiceManager();

	sql::Schema &db();

private:
	tcp::acceptor _acceptor;
	asio::ssl::context _context;

	sql::Session _sess;
	sql::Schema _db;

	std::shared_ptr<UserManager> _user_manager;
	std::shared_ptr<EventManager> _event_manager;
	std::shared_ptr<ChannelManager> _channel_manager;
	std::shared_ptr<MessageManager> _message_manager;
	std::shared_ptr<SessionManager> _session_manager;
	std::shared_ptr<VoiceManager> _voice_manager;

	void acceptClient();
};