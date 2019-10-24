#pragma once

#include <memory>
#include <string>

#include "communicator.h"
#include "auth.h"
#include "users.h"
#include "channels.h"
#include "messages.h"
#include "voice.h"
#include "files.h"
#include "user.h"

namespace quesync {
namespace client {
class client : public std::enable_shared_from_this<client> {
   public:
    client();

    void init();

    void connect(std::string server_ip);

    std::shared_ptr<modules::communicator> communicator();
    std::shared_ptr<modules::auth> auth();
    std::shared_ptr<modules::users> users();
    std::shared_ptr<modules::channels> channels();
    std::shared_ptr<modules::messages> messages();
    std::shared_ptr<modules::voice> voice();
    std::shared_ptr<modules::files> files();
    std::shared_ptr<modules::user> user();

    void clean_connection();
    void logged_out();

   private:
    std::shared_ptr<modules::communicator> _communicator;
    std::shared_ptr<modules::auth> _auth;
    std::shared_ptr<modules::users> _users;
    std::shared_ptr<modules::channels> _channels;
    std::shared_ptr<modules::messages> _messages;
    std::shared_ptr<modules::voice> _voice;
    std::shared_ptr<modules::files> _files;
    std::shared_ptr<modules::user> _user;
};
};  // namespace client
};  // namespace quesync