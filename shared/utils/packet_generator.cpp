#include "packet_generator.h"

const std::unordered_map<quesync::packet_type, std::function<std::shared_ptr<quesync::packet>()>>
    quesync::utils::packet_generator::packet_initalizers{
        PACKET_ENTRY(ping_packet),
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
        PACKET_ENTRY(get_channel_calls_packet),
        PACKET_ENTRY(upload_file_packet),
        PACKET_ENTRY(download_file_packet),
        PACKET_ENTRY(get_file_info_packet),
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
        RESPONSE_PACKET_ENTRY(channel_calls_packet),
        RESPONSE_PACKET_ENTRY(file_upload_initiated_packet),
        RESPONSE_PACKET_ENTRY(file_download_initiated_packet),
        RESPONSE_PACKET_ENTRY(file_info_packet),
        RESPONSE_PACKET_ENTRY(pong_packet)};