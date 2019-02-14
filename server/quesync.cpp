#include <iostream>

#include "quesync.h"
#include "utils.h"
#include "session.h"
#include "quesync_exception.h"

Quesync::Quesync(asio::io_context& io_context) : _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT))
{
    // Initialize the database
    initDB();
}

Quesync::~Quesync()
{
    // Close all socket handlers
    _acceptor.cancel();

    // Free the database and close it
    delete _db;
}

void Quesync::initDB()
{
    std::cout << "Connecting to database.." << std::endl;

    // Connecting to the quesync database
    _db = new sqlitepp::db("db/quesync-db.db");

    // If the connection to the database failed
    if (!_db->is_open())
    {
        throw std::string("Failed to connect to the SQLite database!");
    } else 
    {
        std::cout << "SQLite Connection was successful!" << std::endl;
    }

    std::cout << "Initializing SQLite database.." << std::endl;

    // Create the users table if not exists
    sqlitepp::query(*_db, "CREATE TABLE IF NOT EXISTS users (id TEXT PRIMARY KEY UNIQUE NOT NULL, username TEXT NOT NULL, password TEXT NOT NULL, email TEXT NOT NULL, nickname TEXT)").exec();
}

void Quesync::start()
{
    std::cout << "Listening for TCP connections.." << std::endl;

    // Start acception requests
    acceptClient();
}

void Quesync::acceptClient()
{
    // Start an async accept
    _acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            // If no error occurred during the connection to the client start a session with it
            if (!ec)
            {
                // Print the client ip and port
                std::cout << "Client connected from " << socket.remote_endpoint().address().to_string() << ":" << (int)socket.remote_endpoint().port() << std::endl;

                // Create a shared session for the client socket
                std::make_shared<Session>(std::move(socket), this)->start();
            } else {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }

            // Accept the next client
            acceptClient();
        });
}

User *Quesync::authenticateUser(std::string username, std::string password)
{
    User *user = nullptr;

    sqlitepp::query userQuery(*_db, "SELECT * FROM users WHERE username=?");
    sqlitepp::result userRes;

    // Try to get the password hash of the user
    userQuery.bind(1, username);
    userRes = userQuery.store();

    // If the user is not found
    if (userRes.size() == 0)
    {
        throw QuesyncException(USER_NOT_FOUND);
    } 
    // If the password the user entered doesn't match the user's password
    else if (Utils::SHA256(password) != std::string(userRes[0]["password"]))
    {
        throw QuesyncException(INCORRECT_PASSWORD);
    }

    // Create the user from the db response
    user = new User(userRes[0]["username"],
                    userRes[0]["email"],
                    userRes[0]["nickname"],
                    userRes[0]["id"]);

    return user;
}