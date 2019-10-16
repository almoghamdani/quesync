#pragma once

#include <sstream>
#include <string>

#include "../file_chunk.h"
#include "../packet.h"
#include "../utils/memory.h"

#define FILE_CHUNK_ACK_PACKET_HEADER "QUESYNC_FILE_CHUNK_ACK"
#define FILE_ID_SIZE 36

namespace quesync {
namespace packets {
struct file_chunk_ack_packet_format {
    const char header[sizeof(FILE_CHUNK_ACK_PACKET_HEADER)] = FILE_CHUNK_ACK_PACKET_HEADER;
    char file_id[FILE_ID_SIZE];
    unsigned long long next_index;
    bool done;
};

class file_chunk_ack_packet {
   public:
    file_chunk_ack_packet(){};
    file_chunk_ack_packet(std::string file_id, unsigned long long next_index, bool done)
        : _file_id(file_id), _next_index(next_index), _done(done) {}

    std::string encode() {
        file_chunk_ack_packet_format format;

        // Set the file id and seq of the next file chunk
        memcpy(format.file_id, _file_id.data(), FILE_ID_SIZE);
        format.next_index = _next_index;
        format.done = _done;

        return std::string((char *)&format, sizeof(file_chunk_ack_packet_format));
    }

    bool decode(std::string buf) {
        file_chunk_ack_packet_format format;

        // If incorrect size of packet, ignore
        if (buf.length() != sizeof(file_chunk_ack_packet_format)) {
            return false;
        } else if (buf.substr(0, sizeof(FILE_CHUNK_ACK_PACKET_HEADER) - 1) !=
                   FILE_CHUNK_ACK_PACKET_HEADER)  // Check header
        {
            return false;
        }

        // Copy the packet data to the packet format
        memcpy(&format, buf.data(), sizeof(file_chunk_ack_packet_format));

        // Get the data from the format
        _file_id = std::string(format.file_id, FILE_ID_SIZE);
        _next_index = format.next_index;
        _done = format.done;

        return true;
    }

    unsigned long long next_index() const { return _next_index; }
    std::string file_id() const { return _file_id; }
    bool done() const { return _done; }

   private:
    unsigned long long _next_index;
    std::string _file_id;
    bool _done;
};
};  // namespace packets
};  // namespace quesync