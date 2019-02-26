#pragma once
#include <exception>
#include <string>

#include "errors.h"

#define MAX_ERROR_MSG_LEN 100

class QuesyncException : public std::exception
{
public:
    QuesyncException(QuesyncError ec) : _ec(ec)
    {
        // Set the error message by the error code
        sprintf(_error_msg, "An error occurred, error code: %d", ec);
    };

    virtual const char* what() const throw (){
       return _error_msg;
    }

    QuesyncError getErrorCode()
    {
        return _ec;
    }

private:
    char _error_msg[MAX_ERROR_MSG_LEN];
    QuesyncError _ec;
};