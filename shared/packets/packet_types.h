#pragma once

typedef enum PacketType
{
    // Requests
    LOGIN_PACKET = 1,
    REGISTER_PACKET,
    SEARCH_PACKET,

    // Respones
    AUTHENTICATED_PACKET = 200,
    SEARCH_RESULTS_PACKET,

    // When an error occurred
    ERROR_PACKET = 400
} PacketType;