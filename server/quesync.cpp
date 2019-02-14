#include <iostream>

#include "quesync.h"
#include "utils.h"
#include "session.h"

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
    sqlitepp::query(*_db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, username TEXT, password TEXT)").exec();
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

QuesyncError Quesync::authenticateUser(std::string username, std::string password)
{
    sqlitepp::query userQuery(*_db);
    sqlitepp::result userRes;
    std::string userPasswordHash;

    // Try to get the password hash of the user
    userQuery << "SELECT password FROM users WHERE username=" << username;
    userRes = userQuery.store();

    // If the user is not found
    if (userRes.size() == 0)
    {
        return USER_NOT_FOUND;
    } 
    
    // If the password the user entered doesn't match the user's password
    userPasswordHash = userRes[0]["password"];
    if (Utils::SHA256(password) != userPasswordHash)
    {
        return INCORRECT_PASSWORD;
    }

    return SUCCESS;
}