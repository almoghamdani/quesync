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
    ResponePacket responePacket;

    // Create a response packet with authenticated code
    responePacket = ResponsePacket(AUTHENTICATED_PACKET);

    // Encode the response packet
    return responsePacket.encode();
}

std::string LoginPacket::encode()
{
    return std::string("");
}