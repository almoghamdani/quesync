#pragma once
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>

#include "server.h"

#include "../shared/user.h"

using asio::ip::tcp;

namespace quesync {
namespace server {
class session : public std::enable_shared_from_this<session> {
   public:
    session(tcp::socket socket, asio::ssl::context &context, std::shared_ptr<server> server);
    ~session();

    void start();
    void send_only(std::string data);

    std::shared_ptr<server> server() const;
    std::shared_ptr<session> get_shared();

    bool authenticated() const;
    void set_user(std::shared_ptr<user> user);
    std::shared_ptr<user> user() const;

   private:
    std::shared_ptr<quesync::user> _user;
    std::shared_ptr<quesync::server::server> _server;

    asio::ssl::stream<tcp::socket> _socket;
    tcp::endpoint _endpoint;

    void clean_user_session();

    void handshake();
    void recv();
    void send(std::string data);
};
};  // namespace server
};  // namespace quesync
