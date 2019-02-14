#pragma once

typedef enum PacketType
{
    // Requests
    LOGIN_PACKET = 1,

    // Respones
    AUTHENTICATED_PACKET = 200,

    // When an error occurred
    QUESYNC_ERROR = 400
} PacketType;