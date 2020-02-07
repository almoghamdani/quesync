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

#define DATABASE_DUMP_TEMP_FILE_NAME "temp_dump"

using asio::ip::tcp;

namespace quesync {
namespace server {
class server : public std::enable_shared_from_this<server> {
   public:
    /**
     * @param io_context The I/O context.
     * @param sql_server_ip The IP of the SQL server.
     * @param sql_username The username to connect with to the SQL server.
     * @param sql_password The password to connect with to the SQL server.
     */
    server(asio::io_context &io_context, std::string sql_server_ip, std::string sql_username,
           std::string sql_password);
    ~server();

    /**
     * Starts the Quesync server.
     */
    void start();

    /**
     * Gets the I/O context.
     *
     * @return The I/O context.
     */
    asio::io_context &get_io_context();

    /**
     * Gets the SSL/TLS context.
     *
     * @return The SSL/TLS socket.
     */
    asio::ssl::context &get_ssl_context();

    /**
     * Gets the shared pointer to the user manager.
     *
     * @return A shared pointer to the user manager.
     */
    std::shared_ptr<user_manager> user_manager();

    /**
     * Gets the shared pointer to the event manager.
     *
     * @return A shared pointer to the event manager.
     */
    std::shared_ptr<event_manager> event_manager();

    /**
     * Gets the shared pointer to the channel manager.
     *
     * @return A shared pointer to the channel manager.
     */
    std::shared_ptr<channel_manager> channel_manager();

    /**
     * Gets the shared pointer to the message manager.
     *
     * @return A shared pointer to the message manager.
     */
    std::shared_ptr<message_manager> message_manager();

    /**
     * Gets the shared pointer to the session manager.
     *
     * @return A shared pointer to the session manager.
     */
    std::shared_ptr<session_manager> session_manager();

    /**
     * Gets the shared pointer to the voice manager.
     *
     * @return A shared pointer to the voice manager.
     */
    std::shared_ptr<voice_manager> voice_manager();

    /**
     * Gets the shared pointer to the file manager.
     *
     * @return A shared pointer to the file manager.
     */
    std::shared_ptr<file_manager> file_manager();

    /**
     * Gets the SQL session.
     *
     * @return The SQL session.
     */
    sql::Session get_sql_session();

    /**
     * Gets the SQL scheme for the session.
     *
     * @param session A reference to the SQL session.
     * @return The SQL scheme.
     */
    sql::Schema get_sql_schema(sql::Session &session);

   private:
    /// The main TCP acceptor for the server.
    tcp::acceptor _acceptor;

    /// The SSL/TLS context.
    asio::ssl::context _context;

    /// The SQL client used to create sessions with the SQL server.
    sql::Client _sql_cli;

    /// A shared pointer to the user manager object.
    std::shared_ptr<quesync::server::user_manager> _user_manager;

    /// A shared pointer to the event manager object.
    std::shared_ptr<quesync::server::event_manager> _event_manager;

    /// A shared pointer to the channel manager object.
    std::shared_ptr<quesync::server::channel_manager> _channel_manager;

    /// A shared pointer to the message manager object.
    std::shared_ptr<quesync::server::message_manager> _message_manager;

    /// A shared pointer to the session manager object.
    std::shared_ptr<quesync::server::session_manager> _session_manager;

    /// A shared pointer to the voice manager object.
    std::shared_ptr<quesync::server::voice_manager> _voice_manager;

    /// A shared pointer to the file manager object.
    std::shared_ptr<quesync::server::file_manager> _file_manager;

    void accept_client();

    static void import_database(std::string sql_server_ip, std::string sql_username,
                                std::string sql_password);

    static std::string format_uri(std::string sql_server_ip, std::string sql_username,
                                  std::string sql_password);

    static std::string format_import_string_unix(std::string sql_server_ip,
                                                 std::string sql_username,
                                                 std::string sql_password);
    static std::string format_import_string_win(std::string sql_server_ip, std::string sql_username,
                                                std::string sql_password);
    static std::string to_hex_string(std::string str);
};
};  // namespace server
};  // namespace quesync
