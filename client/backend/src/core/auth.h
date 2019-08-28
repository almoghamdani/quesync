#pragma once
#include "module.h"

#include <string>

#include "../../../../shared/user.h"

namespace quesync {
namespace client {
namespace modules {
class auth : public module {
   public:
    auth(std::shared_ptr<client> client);

    std::string login(std::string username, std::string password);
    std::string signup(std::string username, std::string password, std::string email);
    void session_auth(std::string session_id);

    std::shared_ptr<user> get_user();

    virtual void clean();

   private:
    std::shared_ptr<user> _user;
};
};  // namespace modules
};  // namespace client
};  // namespace quesync