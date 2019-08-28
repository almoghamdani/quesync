#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "../packet.h"

#define OTP_SIZE 64
#define OTP_PACKET_SIZE 76
#define OTP_HEADER_SIZE 12

namespace quesync {
namespace packets {
class voice_otp_packet {
   public:
    voice_otp_packet(){};
    voice_otp_packet(std::string otp) : _otp(otp) {}

    std::string encode() {
        std::stringstream encoded_packet;

        encoded_packet << "QUESYNC" << PACKET_DELIMETER;
        encoded_packet << "OTP" << PACKET_DELIMETER;
        encoded_packet << _otp;

        return encoded_packet.str();
    }

    bool decode(std::string buf) {
        // If incorrect size of packet, ignore
        if (buf.length() != OTP_PACKET_SIZE) {
            return false;
        } else if (buf.substr(0, OTP_HEADER_SIZE) != "QUESYNC|OTP|")  // Check header
        {
            return false;
        }

        // Get OTP
        _otp = buf.substr(OTP_HEADER_SIZE, OTP_SIZE);

        return true;
    }

    std::string otp() { return _otp; }

   private:
    std::string _otp;
};
};  // namespace packets
};  // namespace quesync