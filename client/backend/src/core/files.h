#pragma once
#include "module.h"

#include <atomic>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "../../../../shared/events/file_transmission_progress_event.h"
#include "../../../../shared/file.h"
#include "../../../../shared/memory_file.h"
#include "socket_manager.h"

#define FILES_SERVER_PORT 61112

#define MAX_PACKETS_IN_BUFFER 100

#define EVENTS_THREAD_SLEEP 60

namespace quesync {
namespace client {
namespace modules {
class files : public module {
   public:
    /**
     * Files module constructor.
     *
     * @param client A sharted pointer to the client object.
     */
    files(std::shared_ptr<client> client);

    /**
     * Starts to upload a file to the file server.
     *
     * @param file_path The path of the file in the local filesystem.
     * @return A shared pointer to the generated file object.
     */
    std::shared_ptr<file> start_upload(std::string file_path);

    /**
     * Starts to download a file from the file server.
     *
     * @param file_id The id of the file.
     * @param download_path The path to download the file to.
     */
    void start_download(std::string file_id, std::string download_path);

    /**
     * Stops file upload/download.
     *
     * @param file_id The id of the file to stop it's transmission.
     */
    void stop_file_transmission(std::string file_id);

    /**
     * Gets the info of a file.
     *
     * @param file_id The id of the file.
     * @return A shared pointer to the file object.
     */
    std::shared_ptr<file> get_file_info(std::string file_id);

    virtual void clean_connection();
    virtual void logged_out();

   private:
    std::shared_ptr<asio::io_context> _io_context;

    asio::ssl::stream<tcp::socket> *_socket;

    std::mutex _uploads_mutex;
    std::unordered_map<std::string, unsigned long long> _uploads_progress;

    std::mutex _downloads_mutex;
    std::unordered_map<std::string, std::shared_ptr<memory_file>> _download_files;
    std::unordered_map<std::string, std::string> _download_paths;

    std::mutex _events_mutex;
    std::unordered_map<std::string, std::shared_ptr<events::file_transmission_progress_event>>
        _events;

    std::thread _events_thread;
    std::thread _io_thread;

    std::atomic<bool> _stop_threads;

    void connect_to_file_server();
    void recv();
    void handle_packet(std::string buf);

    void events_thread();

    void upload(std::shared_ptr<memory_file> file_info);
    void handle_upload_chunk_sent(std::shared_ptr<memory_file> file_info);

    void check_if_idle();
    void shutdown_socket_async();

    void save_file(std::shared_ptr<memory_file> file, std::string download_path);
    std::string get_file_name(std::string file_path);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync