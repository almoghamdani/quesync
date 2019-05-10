#include "user_management.h"

#include <sole.hpp>

#include "session.h"
#include "../shared/utils.h"
#include "../shared/validation.h"
#include "../shared/quesync_exception.h"

UserManagement::UserManagement(sql::Schema &db) : users_table(db, "users"),
                                                  friendships_table(db, "friendships"),
                                                  profiles_table(db, "profiles")
{
}

User *UserManagement::authenticateUser(std::shared_ptr<Session> sess, std::string username, std::string password)
{
    User *user = nullptr;

    sql::Row user_res;

    // Search for the user row in the database
    user_res = users_table.select("*")
                   .where("username = :username")
                   .bind("username", username)
                   .execute()
                   .fetchOne();

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
                    user_res[5],
                    getFriends(user_res[0]));

    // Add the user to the authenticated sessions
    _authenticated_sessions[user_res[0]] = sess;

    return user;
}

User *UserManagement::registerUser(std::shared_ptr<Session> sess,
                                   std::string username,
                                   std::string password,
                                   std::string email,
                                   std::string nickname)
{
    User *user = nullptr;
    std::string id, password_hashed;
    int tag;

    sql::Row user_res;

    // Check if the entered username is a valid username
    if (!Validation::IsValidUsername(username))
    {
        throw QuesyncException(INVALID_USERNAME);
    }

    // Check if the entered e-mail is a valid e-mail
    if (!Validation::IsValidEmail(email))
    {
        throw QuesyncException(INVALID_EMAIL);
    }

    // Check if there are any users with the username/email/nickname of the new user
    try
    {
        user_res = users_table.select("*")
                       .where("username = :username OR email = :email")
                       .bind("username", username)
                       .bind("email", email)
                       .execute()
                       .fetchOne();
    }
    catch (...)
    {
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

        try
        {
            // Insert the new user to the users table
            users_table.insert("id", "username", "password", "email", "nickname", "tag")
                .values(id, username, password_hashed, email, nickname, tag)
                .execute();
        }
        catch (...)
        {
            throw QuesyncException(UNKNOWN_ERROR);
        }

        // Create the object for the user
        user = new User(id, username, email, nickname, tag, std::vector<std::string>());

        // Add the user to the authenticated sessions
        _authenticated_sessions[id] = sess;
    }
    else
    {
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
        else
        {
            throw QuesyncException(UNKNOWN_ERROR);
        }
    }

    return user;
}

Profile *UserManagement::getUserProfile(std::string user_id)
{
    Profile *profile = nullptr;

    sql::Row profile_res;

    // Search for the profile row in the database
    profile_res = profiles_table.select("*")
                      .where("id = :user_id")
                      .bind("user_id", user_id)
                      .execute()
                      .fetchOne();

    // If the user's profile is not found
    if (profile_res.isNull())
    {
        throw QuesyncException(USER_NOT_FOUND);
    }

    // Create the profile from the db response
    profile = new Profile(profile_res[0],
                          profile_res[1],
                          profile_res[2]);

    return profile;
}

std::vector<std::string> UserManagement::getFriends(std::string user_id)
{
    std::vector<std::string> friends;
    sql::RowResult res;
    sql::Row row;

    try
    {
        // Get the ids of the user's friends
        res = friendships_table.select("requester_id", "recipient_id").where("(requester_id = :user_id OR recipient_id = :user_id) AND approved = True").bind("user_id", user_id).execute();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }

    // For each friend of the user, add it to the list of friends
    while ((row = res.fetchOne()))
    {
        // If the "requester_id" column is the user's id, get the friend id from the "recipient_id" column
        if ((std::string)row[0] == user_id)
        {
            friends.push_back(row[1]);
        }
        else
        {
            friends.push_back(row[0]);
        }
    }

    return friends;
}

void UserManagement::sendFriendRequest(std::string requester_id, std::string recipient_id)
{
    // Check if the recipient exists
    if (!users_table.select("1").where("id = :id").bind("id", recipient_id).execute().count())
    {
        throw QuesyncException(USER_NOT_FOUND);
    }

    // Check if the 2 users are already friends
    if (friendships_table.select("1")
            .where("(requester_id = :requester_id AND recipient_id = :recipient_id) OR (requester_id = :recipient_id AND recipient_id = :requester_id)")
            .bind("requester_id", requester_id)
            .bind("recipient_id", recipient_id)
            .execute()
            .count())
    {
        throw QuesyncException(ALREADY_FRIENDS);
    }

    try
    {
        // Add the friend request to the friendships table (approved field is default to false)
        friendships_table.insert("requester_id", "recipient_id")
            .values(requester_id, recipient_id)
            .execute();
    }
    catch (...)
    {
        throw QuesyncException(UNKNOWN_ERROR);
    }
}

void UserManagement::setFriendshipStatus(std::string user_id, std::string friend_id, bool status)
{
    std::string requester, recipient;
    bool approved = false;
    sql::Row friendship_row;

    // Check if the friend exists
    if (!users_table.select("1").where("id = :id").bind("id", friend_id).execute().count())
    {
        throw QuesyncException(USER_NOT_FOUND);
    }

    // Get the friendship between the user and the friend
    friendship_row = friendships_table.select("requester_id", "recipient_id", "approved")
                         .where("(requester_id = :requester_id AND recipient_id = :recipient_id) OR (requester_id = :recipient_id AND recipient_id = :requester_id)")
                         .bind("requester_id", user_id)
                         .bind("recipient_id", friend_id)
                         .execute()
                         .fetchOne();

    // If the user doesn't have the friend as his friend throw an error
    if (friendship_row.isNull())
    {
        throw QuesyncException(NOT_FRIENDS);
    }

    // Get the fields from the row for easier usage
    requester = friendship_row[0].operator std::string();
    recipient = friendship_row[1].operator std::string();
    approved = friendship_row[2];

    // If the user wish to disable a pending friend request or just remove a friend, remove the friendship
    if (status == false)
    {
        try
        {
            // Remove the friendship row
            friendships_table.remove()
                .where("requester_id = :requester_id AND recipient_id = :recipient_id")
                .bind("requester_id", requester)
                .bind("recipient_id", recipient)
                .execute();
        }
        catch (...)
        {
            throw QuesyncException(UNKNOWN_ERROR);
        }
    }
    // If the user is the requester and he is trying to self-approve his friend request
    else if (requester == user_id && approved == false)
    {
        throw QuesyncException(SELF_APPROVE_FRIEND_REQUEST);
    }
    // If the user is trying to approve a friend request where he is already friends with
    else if (approved == status == true)
    {
        throw QuesyncException(ALREADY_FRIENDS);
    }
    else
    {
        try
        {
            // Set the approved field in the friendship row
            friendships_table.update()
                .set("approved", true)
                .where("requester_id = :requester_id AND recipient_id = :recipient_id")
                .bind("requester_id", requester)
                .bind("recipient_id", recipient)
                .execute();
        }
        catch (...)
        {
            throw QuesyncException(UNKNOWN_ERROR);
        }
    }
}