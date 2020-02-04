#pragma once
#include "manager.h"

#include <asio.hpp>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../../shared/memory_file.h"

using namespace std::string_literals;
using asio::ip::tcp;

#define MAX_FILE_SIZE 500 * 1000000
#define FILES_DIR "files"s

#define FILE_SERVER_PORT 61112

#define MAX_PACKETS_IN_BUFFER 100

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
    /**
     * File manager constructor.
     *
     * @param server A shared pointer to the server object.
     */
    file_manager(std::shared_ptr<server> server);

    /**
     * Starts an upload of a file.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param name The name of the file.
     * @param size The size of the file.
     * @return A shared pointer to the generated file object.
     */
    std::shared_ptr<file> init_upload_file(std::shared_ptr<quesync::server::session> sess,
                                           std::string name, unsigned long long size);

    /**
     * Starts a download of a file.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param file_id The id of the file to be downloaded.
     */
    void init_download_file(std::shared_ptr<quesync::server::session> sess, std::string file_id);

    /**
     * Stops a file upload/download.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param file_id The id of the file to stop it's transmission.
     */
    void stop_file_transmission(std::shared_ptr<quesync::server::session> sess,
                                std::string file_id);

    /**
     * Checks if a file exists.
     *
     * @param file_id The id of the file.
     * @return True if the file exists or false otherwise.
     */
    bool does_file_exists(std::string file_id);

    /**
     * Get the info of a f ile.
     *
     * @param file_id The id of the file.
     * @return A shared pointer to the file info object.
     */
    std::shared_ptr<file> get_file_info(std::string file_id);

    /**
     * Get file chunks.
     *
     * @param file_id The id of the file.
     * @return A map of the file's chunks.
     */
    std::unordered_map<unsigned long long, file_chunk> get_file_chunks(std::string file_id);

    /**
     * Save a file to the server's filesystem.
     *
     * @param file A shared pointer to the memory file object.
     */
    void save_file(std::shared_ptr<memory_file> file);

    /**
     * Get a file's content.
     *
     * @param file_id The id of the file.
     * @return The content of the file.
     */
    std::string get_file_content(std::string file_id);

    /**
     * Clear user's memory files(upload and downloads).
     *
     * @param user_id The id of the user.
     */
    void clear_all_user_memory_files(std::string user_id);

    /**
     * Register a file session for user.
     *
     * @param sess A shared pointer to the file session of the user.
     * @param user_id The id of the user.
     */
    void register_user_file_session(std::shared_ptr<file_session> sess, std::string user_id);

   private:
    tcp::acceptor _acceptor;

    std::mutex _sessions_mutex;
    std::unordered_map<std::string, std::shared_ptr<file_session>> _users_file_sessions;

    void accept_client();
};
};  // namespace server
};  // namespace quesync
