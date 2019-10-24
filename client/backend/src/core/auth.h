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

    void logout();

    std::shared_ptr<quesync::user> get_user();
    std::string get_session_id();

    virtual void logged_out();

   private:
    std::shared_ptr<quesync::user> _user;
    std::string _session_id;
};
};  // namespace modules
};  // namespace client
};  // namespace quesync