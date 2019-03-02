#pragma once

typedef enum QuesyncError
{
    SUCCESS,
    UNKNOWN_ERROR,
    INVALID_PACKET,
    USER_NOT_FOUND,
    INCORRECT_PASSWORD,
    ALREADY_AUTHENTICATED,
    INVALID_USERNAME,
    INVALID_EMAIL,
    USERNAME_ALREADY_IN_USE,
    EMAIL_ALREADY_IN_USE,
    NICKNAME_FULL,
    ALREADY_FRIENDS,
    SELF_FRIEND_REQUEST,
    NOT_FRIENDS,
    SELF_APPROVE_FRIEND_REQUEST
} QuesyncError;