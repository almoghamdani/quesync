#include <iostream>

#include "quesync.h"

#include <termcolor/termcolor.hpp>

int main()
{
    asio::io_context io_context;
    std::shared_ptr<Quesync> server;

	std::cout << termcolor::green << termcolor::bold << "Quesync Server v0.1.0" << std::endl;
	std::cout << termcolor::green << termcolor::underline << "All Rights Reserved to Almog Hamdani." << std::endl;
	std::cout << termcolor::reset << std::endl;

    try
    {
        std::cout << termcolor::blue << "Initializing Quesync server.." << termcolor::reset << std::endl;
        // Create the Quesync server
        server = std::make_shared<Quesync>(io_context);

        std::cout << termcolor::blue << "Starting server.." << termcolor::reset << std::endl;
        // Start the server
        server->start();

        // Run the io handler
        io_context.run();
    }
    catch (std::exception &ex)
    {
        std::cout << termcolor::red << "Exception occurred: " << ex.what() << termcolor::reset << std::endl;
    }
    catch (std::string &ex)
    {
        std::cout << termcolor::red << "Exception occurred: " << ex << termcolor::reset << std::endl;
    }
    catch (...)
    {
        std::cout << termcolor::red << "An unknown error has occurred!" << termcolor::reset << std::endl;
    }

    // Wait for key press
    system("pause");

    return 0;
}