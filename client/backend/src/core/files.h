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

#define EVENTS_THREAD_SLEEP 15

namespace quesync {
namespace client {
namespace modules {
class files : public module {
   public:
    files(std::shared_ptr<client> client);

    std::shared_ptr<file> start_upload(std::string file_path);
    void start_download(std::string file_id, std::string download_path);

    std::shared_ptr<file> get_file_info(std::string file_id);

    virtual void clean_connection(bool join_com_thread=true);
    virtual void disconnected();

   private:
    std::mutex _send_mutex;
    asio::ssl::stream<tcp::socket> *_socket;

    std::mutex _data_mutex;
    std::unordered_map<std::string, std::shared_ptr<memory_file>> _upload_files;
    std::unordered_map<std::string, std::shared_ptr<memory_file>> _download_files;
    std::unordered_map<std::string, std::string> _download_paths;
    
    std::mutex _events_mutex;
    std::unordered_map<std::string, std::shared_ptr<events::file_transmission_progress_event>> _events;

    std::thread _com_thread;
    std::thread _events_thread;

    std::atomic<bool> _stop_threads;

    void com_thread();
    void events_thread();

    void connect_to_file_server();

    void init_upload(std::string file_id);

    void save_file(std::shared_ptr<memory_file> file);
    std::string get_file_name(std::string file_path);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync