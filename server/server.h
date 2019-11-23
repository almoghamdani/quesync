#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <map>
#include <memory>
#include <string>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "channel_manager.h"
#include "event_manager.h"
#include "file_manager.h"
#include "message_manager.h"
#include "session_manager.h"
#include "user_manager.h"
#include "voice_manager.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

namespace quesync {
namespace server {
class server : public std::enable_shared_from_this<server> {
   public:
    server(asio::io_context &io_context, std::string sql_server_ip, std::string sql_username, std::string sql_password);
    ~server();

    void start();

    asio::io_context &get_io_context();
    asio::ssl::context &get_ssl_context();

    std::shared_ptr<user_manager> user_manager();
    std::shared_ptr<event_manager> event_manager();
    std::shared_ptr<channel_manager> channel_manager();
    std::shared_ptr<message_manager> message_manager();
    std::shared_ptr<session_manager> session_manager();
    std::shared_ptr<voice_manager> voice_manager();
    std::shared_ptr<file_manager> file_manager();

    sql::Session get_sql_session();
    sql::Schema get_sql_schema(sql::Session &session);

   private:
    tcp::acceptor _acceptor;
    asio::ssl::context _context;

    sql::Client _sql_cli;

    std::shared_ptr<quesync::server::user_manager> _user_manager;
    std::shared_ptr<quesync::server::event_manager> _event_manager;
    std::shared_ptr<quesync::server::channel_manager> _channel_manager;
    std::shared_ptr<quesync::server::message_manager> _message_manager;
    std::shared_ptr<quesync::server::session_manager> _session_manager;
    std::shared_ptr<quesync::server::voice_manager> _voice_manager;
    std::shared_ptr<quesync::server::file_manager> _file_manager;

    void accept_client();

    static std::string format_uri(std::string sql_server_ip, std::string sql_username, std::string sql_password);
};
};  // namespace server
};  // namespace quesync
