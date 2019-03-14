#pragma once
#include "response_packet.h"
#include "../errors.h"

#define ERROR_CODE_LEN 3

class ErrorPacket : public ResponsePacket
{
  public:
    ErrorPacket() : ResponsePacket(ERROR_PACKET, ""){};

    ErrorPacket(QuesyncError ec) : _ec(ec),
                                   ResponsePacket(ERROR_PACKET,
                                                  std::string(ERROR_CODE_LEN - std::to_string(ec).length(), '0') + std::to_string(ec)) // Add leading zeros to the error code
                                   {};

    virtual bool decode(std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        try
        {
            // Decode the error code
            _ec = (QuesyncError)std::stoi(params[0]);
        }
        catch (...)
        {
            return false;
        }

        return true;
    };

    QuesyncError error() const
    {
        return _ec;
    };

  protected:
    QuesyncError _ec;
};