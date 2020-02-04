#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../file.h"

namespace quesync {
namespace packets {
class file_transmission_stop_packet : public serialized_packet {
   public:
    /// Default constructor.
    file_transmission_stop_packet() : file_transmission_stop_packet(""){};

    /**
     * Packet constructor.
     *
     * @param file_id The id of the file.
     */
    file_transmission_stop_packet(std::string file_id)
        : serialized_packet(packet_type::file_transmission_stop_packet) {
        _data["fileId"] = file_id;
    };

    virtual bool verify() const { return exists("fileId"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Stop the file's transmission
            session->server()->file_manager()->stop_file_transmission(session, _data["fileId"]);

            // Return true response packet
            return response_packet(packet_type::file_transmission_stopped_packet).encode();
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