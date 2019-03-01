#include "quesync.h"

#include <iostream>
#include <sole.hpp>

#include "session.h"
#include "../shared/utils.h"
#include "../shared/quesync_exception.h"

Quesync::Quesync(asio::io_context& io_context) : 
    _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT)),
    _sess("localhost", 33060, "server", "123456789"),
    _db(_sess, "quesync")
{
}

Quesync::~Quesync()
{
    // Close all socket handlers
    _acceptor.cancel();

    // Free the database and close it
    //_db->close();
    //delete _db;
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

    sql::Table users_table = _db.getTable("users");
    sql::Row user_res;

    // Search for the user row in the database
    user_res = users_table.select("*")
        .where("username = :username")
        .bind("username", username).execute().fetchOne();

    // If the user is not found
    if (user_res.isNull())
    {
        throw QuesyncException(USER_NOT_FOUND);
    } 
    // If the password the user entered doesn't match the user's password
    else if (Utils::SHA256(password) != std::string(user_res[2]))
    {
        throw QuesyncException(INCORRECT_PASSWORD);
    }

    // Create the user from the db response
    user = new User(user_res[0],
                    user_res[1],
                    user_res[3],
                    user_res[4],
                    user_res[5]);

    return user;
}

User *Quesync::registerUser(std::string username, 
                            std::string password,
                            std::string email,
                            std::string nickname)
{
    User *user = nullptr;
    std::string id, password_hashed;
    int tag;

    sql::Table users_table = _db.getTable("users");
    sql::Row user_res;

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
    try {
        user_res = users_table.select("*")
            .where("username = :username OR email = :email")
            .bind("username", username).bind("email", email).execute().fetchOne();
    } catch (...) {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // If no user found, create the new user
    if (user_res.isNull())
    {
        // Create an ID for the user
        id = sole::uuid4().str();

        // Hash the user's password
        password_hashed = Utils::SHA256(password);

        // Generate the random user tag
        tag = Utils::GenerateTag(nickname, users_table);

        try {
            // Insert the new user to the users table
            users_table.insert("id", "username", "password", "email", "nickname", "tag", "friends")
                .values(id, username, password_hashed, email, nickname, tag, "[]").execute();
        } catch (...) {
            throw QuesyncException(UNKNOWN_ERROR);
        }

        // Create the object for the user
        user = new User(id, username, email, nickname, tag);
    } else {
        // If the username is already taken
        if (std::string(user_res[1]) == username)
        {
            throw QuesyncException(USERNAME_ALREADY_IN_USE);
        }
        // If the email is already taken 
        else if (std::string(user_res[3]) == email) 
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

sql::Schema& Quesync::db()
{
    return _db;
}