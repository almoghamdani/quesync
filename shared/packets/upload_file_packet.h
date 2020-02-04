#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../file.h"

namespace quesync {
namespace packets {
class upload_file_packet : public serialized_packet {
   public:
    /// Default constructor.
    upload_file_packet() : upload_file_packet("", 0){};

    /**
     * Packet constructor.
     *
     * @param name The name of the file.
     * @param size The size of the file.
     */
    upload_file_packet(std::string name, unsigned long long size)
        : serialized_packet(packet_type::upload_file_packet) {
        _data["name"] = name;
        _data["size"] = size;
    };

    virtual bool verify() const { return exists("name") && exists("size"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<quesync::file> file;

        nlohmann::json res;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Initiate the upload for the file
            file = session->server()->file_manager()->init_upload_file(session, _data["name"],
                                                                       _data["size"]);
            res["file"] = *file;

            // Return response packet with the file info
            return response_packet(packet_type::file_upload_initiated_packet, res).encode();
        } catch (exception &ex) {
            // Return the error code
            return error_packet(ex.error_code()).encode();
        } catch (...) {
            return error_packet(error::unknown_error).encode();
        }
    };
#endif
};
};  // namespace packets
};  // namespace quesync