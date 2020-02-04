#pragma once

#include <memory>

namespace quesync {
namespace server {
// Prevent loop header include
class server;

class manager {
   public:
    /**
     * Manager constructor.
     *
     * @param server A shared pointer to the server object.
     */
    manager(std::shared_ptr<server> server) : _server(server) {}

   protected:
    std::shared_ptr<server> _server;
};
};  // namespace server
};  // namespace quesync
