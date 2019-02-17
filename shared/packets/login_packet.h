#pragma once
#include "packet.h"

#include <iostream>
#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class LoginPacket : public Packet
{
public:
    LoginPacket() : LoginPacket("", "") {};

    LoginPacket(std::string username, std::string password) : Packet(LOGIN_PACKET)
    {
        _data["username"] = username;
        _data["password"] = password;
    };

    virtual std::string encode()
    {
        // Format the login packet
        return (std::stringstream() << PACKET_IDENTIFIER << PACKET_DELIMETER
                                   << std::setw(3) << std::setfill('0') << LOGIN_PACKET << PACKET_DELIMETER
                                   << _data.dump() << PACKET_DELIMETER).str();
    };

    virtual bool decode (std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        // Try to parse the data as a json
        try {
            _data = nlohmann::json::parse(params[0]);

            // Check if a valid data has entered (username and password)
            if (_data.find("username") == _data.end() || _data.find("password") == _data.end())
            {
                throw "";
            }
        } catch (...) {
            return false;
        }

        return true;
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        User *user;

        try {
            // Authenticate the usetr, if failed an exception will be thrown
            user = session->server()->authenticateUser(_data["username"], _data["password"]);

            // Set the user in the client's session
            session->setUser(user);

            // Return autheticated packet with the user's info
            return ResponsePacket(AUTHENTICATED_PACKET, user->serialize()).encode();
        } catch (QuesyncException& ex) {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif

private:
    nlohmann::json _data;
};