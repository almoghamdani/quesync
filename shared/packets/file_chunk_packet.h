#pragma once

#include <sstream>
#include <string>

#include "../file_chunk.h"
#include "../packet.h"
#include "../utils/memory.h"

#define FILE_CHUNK_PACKET_HEADER "QUESYNC_FILE_CHUNK"
#define FILE_ID_SIZE 36

namespace quesync {
namespace packets {
struct file_chunk_packet_format {
    /// The header of the packet.
    const char header[sizeof(FILE_CHUNK_PACKET_HEADER)] = FILE_CHUNK_PACKET_HEADER;

    /// The id of the file.
    char file_id[FILE_ID_SIZE];

    /// The data of the file chunk.
    char data[FILE_CHUNK_SIZE];

    /// The index of the file chunk.
    unsigned long long index;
};

class file_chunk_packet {
   public:
    /// Default constructor.
    file_chunk_packet(){};

    /**
     * Packet constructor.
     *
     * @param file_id The id of the file.
     * @param chunk The file chunk.
     */
    file_chunk_packet(std::string file_id, file_chunk chunk) : _file_id(file_id), _chunk(chunk) {}

    /**
     * Encodes the packet.
     *
     * @return The packet encoded.
     */
    std::string encode() {
        file_chunk_packet_format format;

        // Set the file id, data and index of the file chunk
        memcpy(format.file_id, _file_id.data(), FILE_ID_SIZE);
        memcpy(format.data, _chunk.data.get(), FILE_CHUNK_SIZE);
        format.index = _chunk.index;

        return std::string((char *)&format, sizeof(file_chunk_packet_format));
    }

    /**
     * Decode the packet.
     *
     * @param buf The packet's encoded data.
     * @return True if the packet was decoded successfully or false otherwise.
     */
    bool decode(std::string buf) {
        file_chunk_packet_format format;

        // If incorrect size of packet, ignore
        if (buf.length() != sizeof(file_chunk_packet_format)) {
            return false;
        } else if (buf.substr(0, sizeof(FILE_CHUNK_PACKET_HEADER) - 1) !=
                   FILE_CHUNK_PACKET_HEADER)  // Check header
        {
            return false;
        }

        // Copy the packet data to the packet format
        memcpy(&format, buf.data(), sizeof(file_chunk_packet_format));

        // Get the data from the format
        _file_id = std::string(format.file_id, FILE_ID_SIZE);
        _chunk = {utils::memory::convert_to_buffer<unsigned char>(
                      std::string(format.data, FILE_CHUNK_SIZE)),
                  format.index};

        return true;
    }

    /**
     * Get the file chunk.
     *
     * @return The file chunk.
     */
    const file_chunk &chunk() const { return _chunk; }

    /**
     * Get the file id.
     *
     * @return The file id.
     */
    std::string file_id() const { return _file_id; }

   private:
    file_chunk _chunk;
    std::string _file_id;
};
};  // namespace packets
};  // namespace quesync