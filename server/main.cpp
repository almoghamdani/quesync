#include <chrono>
#include <iostream>
#include <thread>

#include "server.h"

// Include this after the server to avoid duplicate includes of WinSock on windows
#include <termcolor/termcolor.hpp>

#define NUM_OF_THREADS 6

int main() {
    asio::io_context io_context;
    std::vector<std::thread> threads;

    std::shared_ptr<quesync::server::server> server;

    std::cout << termcolor::green << termcolor::bold << "Quesync Server v0.1.0"
              << "\n";
    std::cout << termcolor::green << termcolor::underline << "All Rights Reserved to Almog Hamdani."
              << "\n";
    std::cout << termcolor::reset << "\n";

    try {
        std::cout << termcolor::blue << "Initializing Quesync server.." << termcolor::reset << "\n";

        // Create the Quesync server
        server = std::make_shared<quesync::server::server>(io_context);

        std::cout << termcolor::blue << "Starting threads.." << termcolor::reset << "\n";

        // Start the threads
        for (int i = 0; i < NUM_OF_THREADS; i++) {
            std::cout << termcolor::blue << "Starting thread no. " << i + 1 << ".."
                      << termcolor::reset << "\n";

            threads.push_back(std::thread([&] {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    try {
                        io_context.run();
                    } catch (std::exception &ex) {
                        std::cout << termcolor::red << "Exception occurred in thread "
                                  << std::this_thread::get_id() << " : " << ex.what()
                                  << termcolor::reset << "\n";
                    } catch (...) {
                        std::cout << termcolor::red << "An unknown error has occurred in thread "
                                  << std::this_thread::get_id() << "!" << termcolor::reset << "\n";
                    }
                }
            }));
        }

        std::cout << termcolor::blue << "Starting server.." << termcolor::reset << "\n";

        // Start the server
        server->start();

        // Join the threads
        for (int i = 0; i < threads.size(); i++) {
            if (threads[i].joinable()) threads[i].join();
        }
    } catch (std::exception &ex) {
        std::cout << termcolor::red << "Exception occurred: " << ex.what() << termcolor::reset
                  << "\n";
    } catch (std::string &ex) {
        std::cout << termcolor::red << "Exception occurred: " << ex << termcolor::reset << "\n";
    } catch (...) {
        std::cout << termcolor::red << "An unknown error has occurred!" << termcolor::reset << "\n";
    }

    return 0;
}