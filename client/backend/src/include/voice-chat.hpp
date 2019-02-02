#pragma once

#ifdef _WIN32
#include <SDKDDKVer.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP // Define min sockets version of Windows XP
#endif

#include "socket-manager.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <uv.h>

#include <al.h>
#include <alc.h>
#include <opus.h>
#include <bass.h>

#define VOICE_CHAT_PORT 61111

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 2
#define FRAMERATE 960
#define MAX_FRAMERATE 4800

#define RECV_BUFFER_SIZE 500

class VoiceChat
{
public:
    VoiceChat(const char *serverIP);
    ~VoiceChat();

private:
    udp::socket _socket;
    udp::endpoint _endpoint;

    std::thread sendThread;
    std::thread recvThread;

    void sendVoiceThread();
    void recvVoiceThread();

    /*uv_udp_t _socket;
    uv_thread_t _sendThread;
    std::string _ipAddress;

    void InitVoiceStreeam();

    static void receiveVoiceThread(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
    static void sendVoiceThread(VoiceChat *voiceChat);*/
};