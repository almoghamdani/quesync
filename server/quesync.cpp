#include "quesync.h"

#include <iostream>
#include <sole.hpp>

#include "session.h"
#include "../shared/utils.h"
#include "../shared/quesync_exception.h"

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

User *Quesync::registerUser(std::string username, 
                            std::string password,
                            std::string email,
                            std::string nickname)
{
    User *user = nullptr;
    std::string id, password_hashed;

    sqlitepp::query userQuery(*_db, "SELECT * FROM users WHERE username=? OR email=?"),
                    newUserQuery(*_db, "INSERT INTO users(id, username, password, email, nickname) VALUES(?, ?, ?, ?, ?)");
    sqlitepp::result userRes;

    // Check if the entered username is a valid username
    if (!Utils::isValidUsername(username))
    {
        throw QuesyncException(INVALID_USERNAME);
    }

    // Check if the entered e-mail is a valid e-mail
    if (!Utils::isValidEmail(email))
    {
        throw QuesyncException(INVALID_EMAIL);
    }

    // Check if there are any users with the username/email/nickname of the new user
    userQuery.bind(1, username);
    userQuery.bind(2, email);
    userRes = userQuery.store();

    // If no user found, create the new user
    if (userRes.size() == 0)
    {
        // Create an ID for the user
        id = sole::uuid4().str();

        // Hash the user's password
        password_hashed = Utils::SHA256(password);

        // Set user's details
        newUserQuery.bind(1, id);
        newUserQuery.bind(2, username);
        newUserQuery.bind(3, password_hashed);
        newUserQuery.bind(4, email);
        newUserQuery.bind(5, nickname);

        // Execute the add query, if failed throw unknown error
        if (newUserQuery.exec())
        {
            throw QuesyncException(UNKNOWN_ERROR);
        } else {
            // Create the object for the user
            user = new User(username, email, nickname, id);
        }
    } else {
        // If the username is already taken
        if (std::string(userRes[0]["username"]) == username)
        {
            throw QuesyncException(USERNAME_ALREADY_IN_USE);
        }
        // If the email is already taken 
        else if (std::string(userRes[0]["email"]) == email) 
        {
            throw QuesyncException(EMAIL_ALREADY_IN_USE);
        }
        // We shouldn't get here
        else {
            throw QuesyncException(UNKNOWN_ERROR);
        }
    }

    return user;
}