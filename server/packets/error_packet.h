#pragma once
#include "response_packet.h"
#include "../errors.h"

class ErrorPacket : public ResponsePacket
{
public:
    ErrorPacket(QuesyncError ec) : _ec(ec), ResponsePacket(QUESYNC_ERROR, std::to_string(ec))
    {
    };

protected:
    QuesyncError _ec;
};