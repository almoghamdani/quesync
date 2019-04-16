#pragma once

#include "socket-manager.hpp"

#include <iostream>
#include <string>
#include <thread>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <opus/opus.h>
#include <bass.h>

#define VOICE_CHAT_PORT 61111

#define RECORD_FREQUENCY 4000
#define RECORD_CHANNELS 2
#define FRAMERATE 960
#define MAX_FRAMERATE 4800

#define RECV_BUFFER_SIZE 500

class VoiceChat
{
  public:
    VoiceChat(const char *server_ip);
    ~VoiceChat();

  private:
    udp::socket _socket;
    udp::endpoint _endpoint;

    std::thread sendThread;
    std::thread recvThread;

    void sendVoiceThread();
    void recvVoiceThread();
};