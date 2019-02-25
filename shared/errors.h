#pragma once

typedef enum QuesyncError
{
    SUCCESS,
    INVALID_PACKET,
    USER_NOT_FOUND,
    INCORRECT_PASSWORD,
    ALREADY_AUTHENTICATED
} QuesyncError;