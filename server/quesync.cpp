#include "quesync.h"

#include <iostream>
#include <termcolor/termcolor.hpp>

#include "session.h"

Quesync::Quesync(asio::io_context &io_context) : _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT)),
												 _context(asio::ssl::context::sslv23),
												 _sess("localhost", 33060, "server", "123456789"),
												 _db(_sess, "quesync")
{
	_context.set_options(
        asio::ssl::context::default_workarounds
        | asio::ssl::context::no_sslv2);
    _context.use_certificate_chain_file("server.pem");
    _context.use_private_key_file("server.pem", asio::ssl::context::pem);

	// Use the quesync database
	_sess.sql("USE quesync").execute();
}

Quesync::~Quesync()
{
	// Close all socket handlers
	_acceptor.cancel();

	// Free the database and close it
	//_db->close();
	//delete _db;
}

void Quesync::start()
{
	// Initialize managers
	_user_manager = std::make_shared<UserManager>(shared_from_this());
	_event_manager = std::make_shared<EventManager>(shared_from_this());
	_channel_manager = std::make_shared<ChannelManager>(shared_from_this());
	_message_manager = std::make_shared<MessageManager>(shared_from_this());
	_session_manager = std::make_shared<SessionManager>(shared_from_this());
	_voice_manager = std::make_shared<VoiceManager>(shared_from_this());

	std::cout << termcolor::cyan << "Listening for TCP connections.." << termcolor::reset << std::endl;

	// Start acception requests
	acceptClient();
}

asio::io_context &Quesync::get_io_context()
{
	return _acceptor.get_io_context();
}

void Quesync::acceptClient()
{
	// Start an async accept
	_acceptor.async_accept(
		[this](std::error_code ec, tcp::socket socket) {
			// If no error occurred during the connection to the client start a session with it
			if (!ec)
			{
				// Print the client ip and port
				std::cout << termcolor::green << "Client connected from " << socket.remote_endpoint().address().to_string() << ":" << (int)socket.remote_endpoint().port() << termcolor::reset << std::endl;

				// Create a shared session for the client socket
				std::make_shared<Session>(std::move(socket), _context, shared_from_this())->start();
			}
			else
			{
				// Print error
				std::cout << termcolor::red << "An error occurred: " << ec << termcolor::reset << std::endl;
			}

			// Accept the next client
			acceptClient();
		});
}

std::shared_ptr<UserManager> Quesync::userManager()
{
	return _user_manager;
}

std::shared_ptr<EventManager> Quesync::eventManager()
{
	return _event_manager;
}

std::shared_ptr<ChannelManager> Quesync::channelManager()
{
	return _channel_manager;
}

std::shared_ptr<MessageManager> Quesync::messageManager()
{
	return _message_manager;
}

std::shared_ptr<SessionManager> Quesync::sessionManager()
{
	return _session_manager;
}

std::shared_ptr<VoiceManager> Quesync::voiceManager()
{
	return _voice_manager;
}

sql::Schema &Quesync::db()
{
	return _db;
}