#pragma once
#include "response_packet.h"
#include "../errors.h"

#define ERROR_CODE_LEN 3

class ErrorPacket : public ResponsePacket
{
public:
    ErrorPacket(QuesyncError ec) : _ec(ec), 
        ResponsePacket(QUESYNC_ERROR, 
                    std::string(ERROR_CODE_LEN - std::to_string(ec).length(), '0') + std::to_string(ec)) // Add leading zeros to the error code
    {
    };

protected:
    QuesyncError _ec;
};