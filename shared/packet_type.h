#pragma once

namespace quesync {
enum class packet_type {
    ping_packet = 0,

    // Requests
    login_packet = 1,
    register_packet,
    profile_request_packet,
    search_packet,
    friend_request_packet,
    friendship_status_packet,
    get_private_channel_packet,
    send_message_packet,
    get_channel_messages_packet,
    session_auth_packet,
    call_request_packet,
    join_call_request_packet,
    leave_call_packet,
    set_voice_state_packet,
    get_channel_calls_packet,
    upload_file_packet,
    download_file_packet,
    get_file_info_packet,

    // Respones
    authenticated_packet = 200,
    search_results_packet,
    friend_request_sent_packet,
    friendship_status_set_packet,
    profile_packet,
    private_channel_packet,
    message_id_packet,
    channel_messages_packet,
    call_started_packet,
    join_call_approved_packet,
    call_left_packet,
    voice_state_set_packet,
    channel_calls_packet,
    file_upload_initiated_packet,
    file_download_initiated_packet,
    file_info_packet,

    // On error
    error_packet = 400,

    // On event
    event_packet = 800,

    pong_packet = 999
};
};
