#pragma once
#include <memory>
#include <asio.hpp>
#include <asio/ssl.hpp>

#include "quesync.h"
#include "../shared/user.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket, asio::ssl::context &context, std::shared_ptr<Quesync> server);
	~Session();

	void start();
	void sendOnly(std::string data);

	std::shared_ptr<Quesync> server() const;
	std::shared_ptr<Session> getShared();

	bool authenticated() const;
	void setUser(std::shared_ptr<User> user);
	std::shared_ptr<User> user() const;

private:
	std::shared_ptr<User> _user;
	std::shared_ptr<Quesync> _server;

	asio::ssl::stream<tcp::socket> _socket;

	void handshake();
	void recv();
	void send(std::string data);
};