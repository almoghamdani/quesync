#pragma once
#include "../response_packet.h"

#include <nlohmann/json.hpp>

#include "events.h"

#define EVENT_CODE_LEN 3

class EventPacket : public ResponsePacket
{
  public:
    EventPacket() : ResponsePacket(EVENT_PACKET, ""){};

    EventPacket(QuesyncEvent evt, nlohmann::json data) : _evt(evt),
                                                         ResponsePacket(EVENT_PACKET,
                                                                        std::string(EVENT_CODE_LEN - std::to_string(evt).length(), '0') + std::to_string(evt) + "|" + data.dump()) // Add leading zeros to the event code + Add the event data
                                                         {};

    virtual bool decode(std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        try
        {
            // Decode the event code
            _evt = (QuesyncEvent)std::stoi(params[0]);

            // Parse the json string
            _data = nlohmann::json::parse(params[1]);
        }
        catch (...)
        {
            return false;
        }

        return true;
    };

    QuesyncEvent event() const
    {
        return _evt;
    };

  protected:
    nlohmann::json _data;
    QuesyncEvent _evt;
};