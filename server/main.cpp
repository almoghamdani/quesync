#include <iostream>

#include "quesync.h"

int main()
{
    asio::io_context io_context;
    std::shared_ptr<Quesync> server;

    try
    {
        std::cout << "Initializing Quesync server.." << std::endl;
        // Create the Quesync server
        server = std::make_shared<Quesync>(io_context);

        std::cout << "Starting server.." << std::endl;
        // Start the server
        server->start();

        // Run the io handler
        io_context.run();
    }
    catch (std::exception &ex)
    {
        std::cout << "Exception occurred: " << ex.what() << std::endl;
    }
    catch (std::string &ex)
    {
        std::cout << "Exception occurred: " << ex << std::endl;
    }
    catch (...)
    {
        std::cout << "An unknown error has occurred!" << std::endl;
    }

    // Wait for key press
    system("pause");

    return 0;
}