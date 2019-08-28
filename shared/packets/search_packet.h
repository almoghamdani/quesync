#pragma once
#include "../serialized_packet.h"

#include <iostream>
#include <vector>

#include "error_packet.h"
#include "../response_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class search_packet : public serialized_packet {
   public:
    search_packet() : search_packet("", -1){};

    search_packet(std::string nickname, int tag) : serialized_packet(packet_type::search_packet) {
        _data["nickname"] = nickname;
        _data["tag"] = tag;
    };

    virtual bool verify() const { return exists("nickname") && exists("tag"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        nlohmann::json results;

        std::string nickname = _data["nickname"];
        int tag = _data["tag"];

        try {
            // Try to search the wanted nickname and tag
            results =
                session->server()->user_manager()->search(session->get_shared(), nickname, tag);
        } catch (...) {
            return error_packet(error::unknown_error).encode();
        }

        try {
            // Return the search results found
            return response_packet(packet_type::search_results_packet, results).encode();
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