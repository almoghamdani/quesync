#include "file_manager.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <sole.hpp>

#ifdef _WIN32
#include <dirent.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "event_manager.h"
#include "file_session.h"
#include "server.h"
#include "session.h"

#include "../shared/exception.h"
#include "../shared/packets/file_chunk_packet.h"
#include "../shared/utils/files.h"
#include "../shared/utils/memory.h"

quesync::server::file_manager::file_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server),
      files_table(server->db(), "files"),
      _acceptor(server->get_io_context(), tcp::endpoint(tcp::v4(), FILE_SERVER_PORT)) {
// Create the files dir
#ifdef _WIN32
    _mkdir(FILES_DIR.c_str());
#else
    mkdir(FILES_DIR.c_str(), 0777);
#endif

    // Start accepting clients
    accept_client();
}

void quesync::server::file_manager::accept_client() {
    // Start an async accept
    _acceptor.async_accept([this](std::error_code ec, tcp::socket socket) {
        // If no error occurred during the connection to the client start a session with it
        if (!ec) {
            // Create a shared file session for the client socket
            std::make_shared<file_session>(std::move(socket), _server->get_ssl_context(), _server)
                ->start();
        } else {
        }

        // Accept the next client
        accept_client();
    });
}

std::shared_ptr<quesync::file> quesync::server::file_manager::init_upload_file(
    std::shared_ptr<quesync::server::session> sess, std::string name, unsigned long long size) {
    std::shared_ptr<file> file;
    std::string file_id = sole::uuid4().str();

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // If the size of the file is bigger than the max
    if (size > MAX_FILE_SIZE) {
        throw exception(error::file_size_exceeded_max);
    }

    // Check if the client is connected to the file server
    if (!_users_file_sessions.count(sess->user()->id)) {
        throw exception(error::file_session_not_connected);
    }

    // Create the memory file object
    file =
        std::make_shared<quesync::file>(file_id, sess->user()->id, name, size, std::time(nullptr));

    // Add the file to upload
    _users_file_sessions[sess->user()->id]->add_upload_file(file);

    return file;
}

void quesync::server::file_manager::init_download_file(
    std::shared_ptr<quesync::server::session> sess, std::string file_id) {
    std::shared_ptr<file> file;

    sql::Row res;

    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // Check if the client is connected to the file server
    if (!_users_file_sessions.count(sess->user()->id)) {
        throw exception(error::file_session_not_connected);
    }

    try {
        // Try to get the file info
        res = files_table.select("id", "uploader_id", "name", "size", "unix_timestamp(uploaded_at)")
                  .where("id = :file_id")
                  .bind("file_id", file_id)
                  .execute()
                  .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the file wasn't found
    if (res.isNull()) {
        throw exception(error::file_not_found);
    }

    // Create the file object
    file = std::make_shared<quesync::file>((std::string)res[0], (std::string)res[1],
                                           (std::string)res[2], (unsigned long long)res[3],
                                           (int)res[4]);

    // Add the file to the download files of the user
    _users_file_sessions[sess->user()->id]->add_download_file(file);
}

void quesync::server::file_manager::stop_file_transmission(
    std::shared_ptr<quesync::server::session> sess, std::string file_id) {
    // Check if the session is authenticated
    if (!sess->authenticated()) {
        throw exception(error::not_authenticated);
    }

    // Check if the client is connected to the file server
    if (!_users_file_sessions.count(sess->user()->id)) {
        throw exception(error::file_session_not_connected);
    }

    // Remove the file from the file session
    _users_file_sessions[sess->user()->id]->remove_file(file_id);
}

bool quesync::server::file_manager::does_file_exists(std::string file_id) {
    try {
        // Check if the file exists
        return files_table.select("1").where("id = :id").bind("id", file_id).execute().count();
    } catch (...) {
        return false;
    }
}

std::shared_ptr<quesync::file> quesync::server::file_manager::get_file_info(std::string file_id) {
    std::shared_ptr<file> file;

    sql::Row res;

    try {
        // Try to get the file info
        res = files_table.select("id", "uploader_id", "name", "size", "unix_timestamp(uploaded_at)")
                  .where("id = :file_id")
                  .bind("file_id", file_id)
                  .execute()
                  .fetchOne();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // If the file wasn't found
    if (res.isNull()) {
        throw exception(error::file_not_found);
    }

    // Create the file object
    file = std::make_shared<quesync::file>((std::string)res[0], (std::string)res[1],
                                           (std::string)res[2], (unsigned long long)res[3],
                                           (int)res[4]);

    return file;
}

void quesync::server::file_manager::clear_all_user_memory_files(std::string user_id) {
    _users_file_sessions.erase(user_id);
}

std::unordered_map<unsigned long long, quesync::file_chunk>
quesync::server::file_manager::get_file_chunks(std::string file_id) {
    std::unordered_map<unsigned long long, quesync::file_chunk> chunks;

    std::ifstream file_stream;
    unsigned long long file_size;

    // Try to open the file for reading
    file_stream.open(FILES_DIR + "/" + file_id, std::ios::binary | std::ios::in);
    if (file_stream.fail()) {
        throw exception(error::file_not_found);
    }

    // Read the entire stream and get the amount of chars read to get the file size
    file_stream.ignore(std::numeric_limits<std::streamsize>::max());
    file_size = file_stream.gcount();
    if (!file_size) {
        throw exception(error::unknown_error);
    }

    // Reset file to beg
    file_stream.clear();  // Reset EOF
    file_stream.seekg(0, std::ios_base::beg);

    // Get the file's chunks
    chunks = utils::files::get_file_chunks(file_stream, file_size);

    return chunks;
}

void quesync::server::file_manager::save_file(std::shared_ptr<quesync::memory_file> file) {
    try {
        // Try to insert the new file entry to the database
        files_table.insert("id", "uploader_id", "name", "size")
            .values(file->file.id, file->file.uploader_id, file->file.name, file->file.size)
            .execute();
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Preform the saving on a different thread to not block the main thread
    std::thread([this, file] {
        std::ofstream fd;
        std::string file_content;

        std::vector<file_chunk> chunks;
        std::vector<std::shared_ptr<unsigned char>> chunks_buffers;

        // Reserve memory for the all the file chunks and the chunk buffers
        chunks.resize(file->chunks.size());
        chunks_buffers.resize(file->chunks.size());

        // Get all the chunks from the chunks map
        std::transform(file->chunks.begin(), file->chunks.end(), chunks.begin(),
                       [](const decltype(file->chunks)::value_type& p) { return p.second; });

        // Sort the chunks
        std::sort(chunks.begin(), chunks.end(),
                  [](const file_chunk& a, const file_chunk& b) { return a.index < b.index; });

        // Get all the buffers of the chunks
        std::transform(chunks.begin(), chunks.end(), chunks_buffers.begin(),
                       [](const file_chunk& p) { return p.data; });

        // Merge the buffers to get the full file content
        file_content = utils::memory::merge_buffers(chunks_buffers, FILE_CHUNK_SIZE);

        // Get only the file content without padding
        file_content = file_content.substr(0, file->file.size);

        // Open the dest file
        fd.open(FILES_DIR + "/" + file->file.id, std::ios::out | std::ios::binary);
        if (fd.fail()) {
            return;
        }

        // Write the file
        fd << file_content;
        fd.close();
    })
        .detach();
}

std::string quesync::server::file_manager::get_file_content(std::string file_id) {
    std::ifstream fd(FILES_DIR + "/" + file_id);
    std::stringstream buffer;

    // Check if the file exists
    if (!does_file_exists(file_id) || fd.fail()) {
        throw exception(error::file_not_found);
    }

    // Read the entire file
    buffer << fd.rdbuf();

    return buffer.str();
}

void quesync::server::file_manager::register_user_file_session(
    std::shared_ptr<quesync::server::file_session> sess, std::string user_id) {
    _users_file_sessions[user_id] = sess;
}