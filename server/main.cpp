#include <iostream>

#include "quesync.h"

int main()
{
    asio::io_context io_context;
    Quesync *server;

    std::cout << "Starting server.." << std::endl;

    try {
        // Create the Quesync server
        server = new Quesync(io_context);

        // Start the server
        server->start();

        // Run the io handler
        io_context.run();
    } catch (std::exception& ex)
    {
        std::cout << "Exception occurred: " << ex.what() << std::endl;
    }

    return 0;
}