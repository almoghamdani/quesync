#pragma once

#include <memory>
#include <string>

namespace quesync {
namespace client {
class client;

class module {
   public:
    module(std::shared_ptr<client> client) : _client(client) {}

    virtual void clean_connection() {}
    virtual void disconnected() {}
    virtual void connected(std::string server_ip) {}

   protected:
    std::shared_ptr<client> _client;
};
};  // namespace client
};  // namespace quesync