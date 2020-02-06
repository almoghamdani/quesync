#pragma once
#include <memory>
#include <string>

#include "auth.h"
#include "channels.h"
#include "communicator.h"
#include "files.h"
#include "messages.h"
#include "user.h"
#include "users.h"
#include "voice.h"

namespace quesync {
namespace client {
class client : public std::enable_shared_from_this<client> {
   public:
    /**
     * Client constructor.
     */
    client();

    /**
     * Initializes the client object.
     */
    void init();

    /**
     * Destroys the client object.
     */
    void destroy();

    /**
     * Connect to a server.
     *
     * @param server_ip The server IP to connect to.
     */
    void connect(std::string server_ip);

    /**
     * Gets the communicator module.
     *
     * @return A shared pointer to the communicator module.
     */
    std::shared_ptr<modules::communicator> communicator();

    /**
     * Gets the auth module.
     *
     * @return A shared pointer to the auth module.
     */
    std::shared_ptr<modules::auth> auth();

    /**
     * Gets the users module.
     *
     * @return A shared pointer to the users module.
     */
    std::shared_ptr<modules::users> users();

    /**
     * Gets the channels module.
     *
     * @return A shared pointer to the channels module.
     */
    std::shared_ptr<modules::channels> channels();

    /**
     * Gets the messages module.
     *
     * @return A shared pointer to the messages module.
     */
    std::shared_ptr<modules::messages> messages();

    /**
     * Gets the voice module.
     *
     * @return A shared pointer to the voice module.
     */
    std::shared_ptr<modules::voice> voice();

    /**
     * Gets the files module.
     *
     * @return A shared pointer to the files module.
     */
    std::shared_ptr<modules::files> files();

    /**
     * Gets the user module.
     *
     * @return A shared pointer to the user module.
     */
    std::shared_ptr<modules::user> user();

    /**
     * Cleans the connection in all modules.
     */
    void clean_connection();

    /**
     * Calls logged out callback in all modules.
     */
    void logged_out();

   private:
    /// A shared pointer to the communicator module.
    std::shared_ptr<modules::communicator> _communicator;

    /// A shared pointer to the auth module.
    std::shared_ptr<modules::auth> _auth;

    /// A shared pointer to the users module.
    std::shared_ptr<modules::users> _users;

    /// A shared pointer to the channels module.
    std::shared_ptr<modules::channels> _channels;

    /// A shared pointer to the messages module.
    std::shared_ptr<modules::messages> _messages;

    /// A shared pointer to the voice module.
    std::shared_ptr<modules::voice> _voice;

    /// A shared pointer to the files module.
    std::shared_ptr<modules::files> _files;

    /// A shared pointer to the user module.
    std::shared_ptr<modules::user> _user;
};
};  // namespace client
};  // namespace quesync