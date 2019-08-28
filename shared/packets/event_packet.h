#pragma once
#include "../response_packet.h"

#include "../event.h"

#include "../utils/event_generator.h"

#define EVENT_CODE_LEN 3

namespace quesync {
namespace packets {
class event_packet : public response_packet {
   public:
    event_packet() : response_packet(packet_type::event_packet, std::string()){};

    event_packet(std::shared_ptr<quesync::event> evt)
        : _evt(evt),
          response_packet(packet_type::event_packet,
                          evt->encode())  // Serialize the event
          {};

    virtual bool decode(std::string packet) {
        // Split the packet
        std::vector<std::string> params = utils::parser::split(packet, PACKET_DELIMETER);

        quesync::event evt;
        nlohmann::json event_json;

        try {
            // Decode the event
            event_json = nlohmann::json::parse(params[0]);

            // Parse it to the event object to get the event type
            evt = event_json;

            // Generate the event from it's type and decode it
            _evt = utils::event_generator::generate_event(evt.type);

            // Decode the json and parse it to the real event object
            _evt->decode(event_json);
        } catch (...) {
            return false;
        }

        return true;
    };

    std::shared_ptr<quesync::event> event() const { return _evt; };

   protected:
    std::shared_ptr<quesync::event> _evt;
};
};  // namespace packets
};  // namespace quesync
