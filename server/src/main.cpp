#include <chrono>
#include <cxxopts.hpp>
#include <iostream>
#include <thread>

#include "server.h"

// Include this after the server to avoid duplicate includes of WinSock on windows
#include <termcolor/termcolor.hpp>

int main(int argc, char *argv[]) {
    asio::io_context io_context;
    std::vector<std::thread> threads;

    std::shared_ptr<quesync::server::server> server;

    // Get the max amount of threads and dividing 1 from it because of the voice states thread
    unsigned int amount_of_threads = std::thread::hardware_concurrency() - 1;

    cxxopts::Options options("Quesync Server",
                             "Cross-Platform Secured VoIP application server optimized for Low Latency");

    // Add default options
    options.add_options()("h,sql-host", "MySQL Server Host / IP",
                          cxxopts::value<std::string>()->default_value("localhost"))(
        "u,sql-username", "MySQL User Name",
        cxxopts::value<std::string>()->default_value("server"))(
        "p,sql-password", "MySQL User Password",
        cxxopts::value<std::string>()->default_value("123456789"))("help", "Print help");

    std::cout << termcolor::green << termcolor::bold << "Quesync Server v1.0.0"
              << "\n";
    std::cout << termcolor::green << termcolor::underline << "All Rights Reserved to Almog Hamdani."
              << "\n";
    std::cout << termcolor::reset << "\n";

    try {
        // Parse options
        auto opts_res = options.parse(argc, argv);

        // Print help
        if (opts_res.count("help")) {
            std::cout << options.help({"", "Group"}) << std::endl;
            return 0;
        }

        std::cout << termcolor::blue << "Initializing Quesync server.." << termcolor::reset << "\n";

        // Create the Quesync server
        server = std::make_shared<quesync::server::server>(
            io_context, opts_res["sql-host"].as<std::string>(),
            opts_res["sql-username"].as<std::string>(), opts_res["sql-password"].as<std::string>());

        std::cout << termcolor::blue << "Testing MySQL connection.." << termcolor::reset << "\n";

        // Test connection to sql server
        server->get_sql_session();

        std::cout << termcolor::blue << "Starting threads.." << termcolor::reset << "\n";

        // Start the threads
        for (unsigned int i = 0; i < amount_of_threads; i++) {
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