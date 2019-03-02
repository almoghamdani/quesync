#pragma once

typedef enum PacketType
{
    // Requests
    LOGIN_PACKET = 1,
    REGISTER_PACKET,
    SEARCH_PACKET,
    FRIEND_REQUEST_PACKET,
    FRIENDSHIP_STATUS_PACKET,

    // Respones
    AUTHENTICATED_PACKET = 200,
    SEARCH_RESULTS_PACKET,
    FRIEND_REQUEST_SENT_PACKET,
    FRIENDSHIP_STATUS_SET_PACKET,

    // When an error occurred
    ERROR_PACKET = 400
} PacketType;