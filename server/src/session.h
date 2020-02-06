#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>
#include <mutex>

#include "server.h"

#include "../../shared/user.h"

using asio::ip::tcp;

namespace quesync {
namespace server {
class session : public std::enable_shared_from_this<session> {
   public:
    /**
     * Session constructor.
     *
     * @param socket The socket of the client.
     * @param context The SSL/TLS context.
     * @param server A shared pointer to the server object.
     */
    session(tcp::socket socket, asio::ssl::context &context, std::shared_ptr<server> server);
    ~session();

    /**
     * Starts the communication with the client.
     */
    void start();

    /**
     * Send a packet to the client.
     *
     * @param data The data of the packet.
     */
    void send_only(std::string data);

    /**
     * Get the shared pointer to the server object.
     *
     * @return A shared pointer to the server object.\
     */
    std::shared_ptr<server> server() const;

    /**
     * Gets a shared pointer to this object.
     *
     * @return A shared pointer to this object.
     */
    std::shared_ptr<session> get_shared();

    /**
     * Checks if the client is autheticated.
     *
     * @return True if the client is authenticated.
     */
    bool authenticated() const;

    /**
     * Sets the user object for the client.
     *
     * @param user A shared pointer to the user object.
     */
    void set_user(std::shared_ptr<user> user);

    /**
     * Gets the user object.
     *
     * @return A shared pointer to the user object.
     */
    std::shared_ptr<user> user() const;

   private:
    /// A shared pointer to the user object.
    std::shared_ptr<quesync::user> _user;

    /// Lock for the user object.
    mutable std::mutex _user_mutex;

    /// A shared pointer to the server object.
    std::shared_ptr<quesync::server::server> _server;

    /// The socket for the user's session.
    asio::ssl::stream<tcp::socket> _socket;

    /// The endpoint of the user.
    tcp::endpoint _endpoint;

    void clean_user_session();

    void handshake();
    void recv();
    void send(std::string data);
};
};  // namespace server
};  // namespace quesync
