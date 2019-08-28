#pragma once

#include <memory>
#include <unordered_map>

#include "../packet_type.h"
#include "../response_packet.h"

#include "../packets/call_request_packet.h"
#include "../packets/event_packet.h"
#include "../packets/friend_request_packet.h"
#include "../packets/friendship_status_packet.h"
#include "../packets/get_channel_messages_packet.h"
#include "../packets/get_private_channel_packet.h"
#include "../packets/join_call_request_packet.h"
#include "../packets/leave_call_packet.h"
#include "../packets/login_packet.h"
#include "../packets/ping_packet.h"
#include "../packets/profile_request_packet.h"
#include "../packets/register_packet.h"
#include "../packets/search_packet.h"
#include "../packets/send_message_packet.h"
#include "../packets/session_auth_packet.h"
#include "../packets/set_voice_state_packet.h"

#define PACKET_ENTRY(packet_name) \
    { packet_type::packet_name, &packet_generator::init_packet<packets::packet_name> }
#define RESPONSE_PACKET_ENTRY(packet_name)                                                 \
    {                                                                                      \
        packet_type::packet_name,                                                          \
            &packet_generator::init_packet<response_packet, (int)packet_type::packet_name> \
    }

namespace quesync {
namespace utils {
class packet_generator {
   public:
    static std::shared_ptr<packet> generate_packet(packet_type type) {
        try {
            return packet_initalizers.at(type)();
        } catch (...) {
            return nullptr;
        }
    }

    template <typename T, int type = -1, typename std::enable_if<type == -1, T>::type* = nullptr>
    static std::shared_ptr<packet> init_packet() {
        return std::shared_ptr<packet>(new T);
    }

    template <typename T, int type = -1, typename std::enable_if<type != -1, T>::type* = nullptr>
    static std::shared_ptr<packet> init_packet() {
        return std::shared_ptr<packet>(new T((packet_type)type, std::string()));
    }

   private:
    inline static const std::unordered_map<packet_type, std::function<std::shared_ptr<packet>()>>
        packet_initalizers = {PACKET_ENTRY(ping_packet),
                              PACKET_ENTRY(login_packet),
                              PACKET_ENTRY(register_packet),
                              PACKET_ENTRY(profile_request_packet),
                              PACKET_ENTRY(search_packet),
                              PACKET_ENTRY(friend_request_packet),
                              PACKET_ENTRY(friendship_status_packet),
                              PACKET_ENTRY(get_private_channel_packet),
                              PACKET_ENTRY(send_message_packet),
                              PACKET_ENTRY(get_channel_messages_packet),
                              PACKET_ENTRY(session_auth_packet),
                              PACKET_ENTRY(call_request_packet),
                              PACKET_ENTRY(join_call_request_packet),
                              PACKET_ENTRY(leave_call_packet),
                              PACKET_ENTRY(set_voice_state_packet),
                              PACKET_ENTRY(event_packet),
                              PACKET_ENTRY(error_packet),
                              RESPONSE_PACKET_ENTRY(authenticated_packet),
                              RESPONSE_PACKET_ENTRY(search_results_packet),
                              RESPONSE_PACKET_ENTRY(friend_request_sent_packet),
                              RESPONSE_PACKET_ENTRY(friendship_status_set_packet),
                              RESPONSE_PACKET_ENTRY(profile_packet),
                              RESPONSE_PACKET_ENTRY(private_channel_packet),
                              RESPONSE_PACKET_ENTRY(message_id_packet),
                              RESPONSE_PACKET_ENTRY(channel_messages_packet),
                              RESPONSE_PACKET_ENTRY(call_started_packet),
                              RESPONSE_PACKET_ENTRY(join_call_approved_packet),
                              RESPONSE_PACKET_ENTRY(call_left_packet),
                              RESPONSE_PACKET_ENTRY(voice_state_set_packet),
                              RESPONSE_PACKET_ENTRY(pong_packet)};
};
};  // namespace utils
};  // namespace quesync