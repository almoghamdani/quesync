#pragma once

#include <memory>

namespace quesync {
namespace server {
// Prevent loop header include
class server;

class manager {
   public:
    manager(std::shared_ptr<server> server) : _server(server) {}

   protected:
    std::shared_ptr<server> _server;
};
};  // namespace server
};  // namespace quesync
