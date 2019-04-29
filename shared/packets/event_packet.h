#pragma once
#include "response_packet.h"

#include "../event.h"

#define EVENT_CODE_LEN 3

class EventPacket : public ResponsePacket
{
  public:
    EventPacket() : ResponsePacket(EVENT_PACKET, ""){};

    EventPacket(Event &evt) : _evt(evt),
                              ResponsePacket(EVENT_PACKET,
                                             evt.serialize()) // Serialize the event
                              {};

    virtual bool decode(std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        try
        {
            // Decode the event
            _evt.deserialize(params[0]);
        }
        catch (...)
        {
            return false;
        }

        return true;
    };

    Event event() const
    {
        return _evt;
    };

  protected:
    Event _evt;
};