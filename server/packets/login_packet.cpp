#include "login_packet.h"
#include "response_packet.h"
#include "..\utils.h"

void LoginPacket::decode (std::string packet)
{
    // Split the packet
    std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

    // Set the email and password from the params
    _email = params[0];
    _password = params[1];
}

std::string LoginPacket::handle (Quesync *server)
{
    // Encode the response packet
    return ResponsePacket(AUTHENTICATED_PACKET).encode();
}

std::string LoginPacket::encode()
{
    return std::string("");
}