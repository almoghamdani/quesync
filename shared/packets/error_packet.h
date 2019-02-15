#pragma once
#include "response_packet.h"
#include "../errors.h"

#define ERROR_CODE_LEN 3

class ErrorPacket : public ResponsePacket
{
public:
    ErrorPacket() {};

    ErrorPacket(QuesyncError ec) : _ec(ec), 
        ResponsePacket(ERROR_PACKET, 
                    std::string(ERROR_CODE_LEN - std::to_string(ec).length(), '0') + std::to_string(ec)) // Add leading zeros to the error code
    {
    };

    virtual bool decode (std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        // Check if the string contains only digits (valid error code)
        if (strspn(params[0].c_str(), "0123456789"))
        {
            // Decode the error code
            _ec = std::to_string(params[0]);

            return true;
        } else {
            return false;
        }
    };

    QuesyncError error() const
    {
        return _ec;
    };

protected:
    QuesyncError _ec;
};