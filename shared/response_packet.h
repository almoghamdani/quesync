#pragma once
#include "packet.h"

#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

#include "utils/parser.h"

namespace quesync {
class response_packet : public packet {
   public:
    response_packet(packet_type type) : response_packet(type, std::string()){};

    response_packet(packet_type type, std::string data) : packet(type), _data(data){};

    response_packet(packet_type type, nlohmann::json json) : packet(type), _data(json.dump()){};

    virtual std::string encode() {
        std::stringstream encoded_packet;

        // Set the packet identifier
        encoded_packet << PACKET_IDENTIFIER << PACKET_DELIMETER;

        // Set the type of the response
        encoded_packet << std::setfill('0') << std::setw(PACKET_TYPE_LEN) << (int)_type
                       << PACKET_DELIMETER;

        // If data was entered add it to the encoded response packet
        if (_data.length()) {
            encoded_packet << _data << PACKET_DELIMETER;
        }

        // Format the response type in the packet
        return encoded_packet.str();
    };

    virtual bool decode(std::string packet) {
        // Split the packet
        std::vector<std::string> params = utils::parser::split(packet, PACKET_DELIMETER);

        if (params.size()) {
            // Get the data from the packet, it should be the first parameter
            _data = params[0];
        }

        try {
            // Try to parse the data as json
            _json = nlohmann::json::parse(_data);
        } catch (...) {
        }

        return true;
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) { return nullptr; };
#endif

    std::string data() const { return _data; };
    nlohmann::json json() const { return _json; };

   protected:
    std::string _data;
    nlohmann::json _json;
};
};  // namespace quesync
