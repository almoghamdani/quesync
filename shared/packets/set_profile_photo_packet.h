#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class set_profile_photo_packet : public serialized_packet {
   public:
    /// Default constructor.
    set_profile_photo_packet() : set_profile_photo_packet(""){};

    /**
     * Packet constructor.
     *
     * @param file_id The id of the profile photo id.
     */
    set_profile_photo_packet(std::string file_id)
        : serialized_packet(packet_type::set_profile_photo_packet) {
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
            // Set the profile photo for the user
            session->server()->user_manager()->set_profile_photo(session, _data["fileId"]);

            // Return confirmation for the profile photo
            return response_packet(packet_type::profile_photo_set_packet).encode();
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
