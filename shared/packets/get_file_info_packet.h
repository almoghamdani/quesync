#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../file.h"

namespace quesync {
namespace packets {
class get_file_info_packet : public serialized_packet {
   public:
    get_file_info_packet() : get_file_info_packet(""){};

    get_file_info_packet(std::string file_id)
        : serialized_packet(packet_type::get_file_info_packet) {
        _data["fileId"] = file_id;
    };

    virtual bool verify() const { return exists("fileId"); };

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
            // Get the file info
            file = session->server()->file_manager()->get_file_info(_data["fileId"]);
            res["file"] = *file;

            // Return response packet with the file info
            return response_packet(packet_type::file_info_packet, res).encode();
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