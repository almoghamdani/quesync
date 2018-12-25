#include <iostream>
#include <exception>
#include <string>

struct SocketError : public std::exception
{
public:
    SocketError(std::string errorMsg, int errorCode) : _errorMsg(errorMsg), _errorCode(errorCode) {}

    const char * what () const throw ()
    {
        std::string exceptionMsg = _errorMsg;

        // Format exception message with the exception and it's error code
        exceptionMsg += ", Error code: ";
        exceptionMsg += std::to_string(_errorCode);

    	return exceptionMsg.c_str();
    }

private:
    std::string _errorMsg;
    int _errorCode;
}