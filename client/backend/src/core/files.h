#pragma once
#include "module.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>

#include "socket_manager.h"

#include "../../../../shared/events/file_transmission_progress_event.h"
#include "../../../../shared/file.h"
#include "../../../../shared/memory_file.h"

#define FILES_SERVER_PORT 61112

#define MAX_PACKETS_IN_BUFFER 100

#define EVENTS_THREAD_SLEEP 60

namespace quesync {
namespace client {
namespace modules {
class files : public module {
   public:
    files(std::shared_ptr<client> client);

    std::shared_ptr<file> start_upload(std::string file_path);
    void start_download(std::string file_id, std::string download_path);

    void stop_file_transmission(std::string file_id);

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
    std::unordered_map<std::string, std::shared_ptr<events::file_transmission_progress_event>> _events;

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

    void save_file(std::shared_ptr<memory_file> file, std::string download_path);
    std::string get_file_name(std::string file_path);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync