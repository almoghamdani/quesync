#include "session.h"

Session::Session(tcp::socket socket, Quesync *server) : 
    _socket(std::move(socket)), // Copy the client's socket
    _server(server) // Save the server for data transfer
{
}

void Session::start()
{

}