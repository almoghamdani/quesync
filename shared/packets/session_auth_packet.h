#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class SessionAuthPacket : public SerializedPacket
{
public:
    SessionAuthPacket() : SessionAuthPacket(""){};

    SessionAuthPacket(std::string session_id) : SerializedPacket(SESSION_AUTH_PACKET)
    {
        _data["sessionId"] = session_id;
    };

    virtual bool verify() const
    {
        return exists("sessionId");
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        std::shared_ptr<User> user;

        // If the user is already authenticated, return error
        if (session->authenticated())
        {
            return ErrorPacket(ALREADY_AUTHENTICATED).encode();
        }

        try
        {
            // Try to get the user id from the session id
            user = session->server()->userManager()->authenticateUserBySession(session->getShared(), _data["sessionId"]);

            // Set the user in the client's session
            session->setUser(user);

            // Return autheticated packet with the user's info
            return ResponsePacket(AUTHENTICATED_PACKET, nlohmann::json{{"user", user->json()}}.dump()).encode();
        }
        catch (QuesyncException &ex)
        {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }
        catch (...)
        {
            return ErrorPacket(UNKNOWN_ERROR).encode();
        }
    };
#endif
};