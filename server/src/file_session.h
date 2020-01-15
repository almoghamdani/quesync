#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "server.h"

#include "../../shared/memory_file.h"
#include "../../shared/user.h"

using asio::ip::tcp;

namespace quesync {
namespace server {
class file_session : public std::enable_shared_from_this<file_session> {
   public:
    file_session(tcp::socket socket, asio::ssl::context& context, std::shared_ptr<server> server);
    ~file_session();

    void start();

    void add_upload_file(std::shared_ptr<file> file);
    void add_download_file(std::shared_ptr<file> file);

    void remove_file(std::string file_id);

   private:
    std::shared_ptr<quesync::user> _user;
    std::shared_ptr<quesync::server::server> _server;

    std::mutex _downloads_mutex;
    std::unordered_map<std::string, unsigned long long> _downloads_progress;

    std::mutex _uploads_mutex;
    std::unordered_map<std::string, std::shared_ptr<memory_file>> _upload_files;

    asio::ssl::stream<tcp::socket> _socket;

    asio::io_context::strand _strand;

    void handshake();
    void recv();
    void send(std::string data);

    std::string handle_packet(std::string buf);
    void handle_download_chunk_sent(std::shared_ptr<memory_file> file_info);
};
};  // namespace server
};  // namespace quesync
