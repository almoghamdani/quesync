#pragma once
#include <napi.h>
#include <uv.h>

#include "voice-chat.hpp"

#define SERVER_PORT 61110
#define MAX_DATA_LEN 1000

using asio::ip::tcp;

class Client : public Napi::ObjectWrap<Client> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        Client(const Napi::CallbackInfo &info);

        void connect(const Napi::CallbackInfo& info);
        void login(const Napi::CallbackInfo& info);

    private:
        static Napi::FunctionReference constructor;

        VoiceChat *_voiceChatManager;
        
        tcp::socket _socket;
        char _data[MAX_DATA_LEN];
};