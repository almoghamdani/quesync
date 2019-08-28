#pragma once
#include <exception>
#include <magic_enum.hpp>
#include <string>

#include "error.h"

#define MAX_ERROR_MSG_LEN 100

namespace quesync {
class exception : public std::exception {
   public:
    exception(error ec) : _ec(ec) {
        // Set the error message by the error code
        sprintf(_error_msg, "An error occurred! Error: quesync::error::%s",
                static_cast<std::string>(magic_enum::enum_name(ec)).c_str());
    };

    virtual const char *what() const throw() { return _error_msg; }

    error error_code() { return _ec; }

   private:
    char _error_msg[MAX_ERROR_MSG_LEN];
    error _ec;
};
};  // namespace quesync
