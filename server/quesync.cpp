#include "quesync.h"

Quesync::Quesync(asio::io_context& io_context) : _socket(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT))
{
}