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
        sprintf(_errorMsg, "An error occurred, error code: %d", ec);
    };

    virtual const char* what() const throw (){
       return _errorMsg;
    }

    QuesyncError getErrorCode()
    {
        return _ec;
    }

private:
    char _errorMsg[MAX_ERROR_MSG_LEN];
    QuesyncError _ec;
};