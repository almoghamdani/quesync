#pragma once
#include <memory>
#include <string>

namespace quesync {
namespace client {
class client;

class module {
   public:
    /**
     * Module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    module(std::shared_ptr<client> client) : _client(client) {}

    /**
     * Cleans the connection for the module.
     */
    virtual void clean_connection() {}

    /**
     * Logged out callback.
     */
    virtual void logged_out() {}

    /**
     * Connected callback.
     *
     * @param server_ip The IP of the server that the client is connected to.
     */
    virtual void connected(std::string server_ip) {}

   protected:
    std::shared_ptr<client> _client;
};
};  // namespace client
};  // namespace quesync