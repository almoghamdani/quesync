#pragma once
#include "manager.h"

#include <asio.hpp>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include <string>
#include <unordered_map>

#include "../shared/memory_file.h"

using namespace std::string_literals;
using asio::ip::tcp;

#define MAX_FILE_SIZE 500 * 1000000
#define FILES_DIR "files"s

#define FILE_SERVER_PORT 61112

namespace quesync {
namespace packets {
class file_chunk_packet;
};

namespace server {
// Prevent loop header include
class session;
class file_session;

class file_manager : manager {
   public:
    file_manager(std::shared_ptr<server> server);

    std::shared_ptr<file> init_upload_file(std::shared_ptr<quesync::server::session> sess,
                                           std::string name, unsigned long long size);
    void init_download_file(std::shared_ptr<quesync::server::session> sess, std::string file_id);

    void stop_file_transmission(std::shared_ptr<quesync::server::session> sess,
                                std::string file_id);

    bool does_file_exists(std::string file_id);
    std::shared_ptr<file> get_file_info(std::string file_id);

    std::unordered_map<unsigned long long, file_chunk> get_file_chunks(std::string file_id);
    void save_file(std::shared_ptr<memory_file> file);

    void clear_all_user_memory_files(std::string user_id);
    void register_user_file_session(std::shared_ptr<file_session> sess, std::string user_id);

   private:
    sql::Table files_table;

    tcp::acceptor _acceptor;

    std::unordered_map<std::string, std::shared_ptr<file_session>> _users_file_sessions;

    void accept_client();
};
};  // namespace server
};  // namespace quesync
