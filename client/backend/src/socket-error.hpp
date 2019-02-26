#include <iostream>
#include <exception>
#include <string>

struct SocketError : public std::exception
{
public:
    SocketError(std::string error_msg, int error_code) : _error_msg(error_msg), _error_code(error_code) {}

    const char * what () const throw ()
    {
        std::string exception_msg = _error_msg;

        // Format exception message with the exception and it's error code
        exception_msg += ", Error code: ";
        exception_msg += std::to_string(_error_code);

    	return exception_msg.c_str();
    }

private:
    std::string _error_msg;
    int _error_code;
};