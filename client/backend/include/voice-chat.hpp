#pragma once
#include <SDKDDKVer.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP // Define min sockets version of Windows XP

#include "socket-manager.hpp"

#include <iostream>
#include <string>
#include <windows.h>

#include <thread>
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
    SOCKET _voiceSocket;

    static void receiveVoiceThread(SOCKET *voiceSocket);
    static void sendVoiceThread(SOCKET *voiceSocket);
};