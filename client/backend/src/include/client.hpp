#pragma once
#include <nan.h>

#include "include/socket-manager.hpp"
#include "include/voice-chat.hpp"

#define SERVER_PORT 61110

class Client : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        Client();

        static void connect(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void login(const Nan::FunctionCallbackInfo<v8::Value>& info);

    private:
        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static Nan::Persistent<v8::Function> constructor;

        VoiceChat *_voiceChatManager;
        uv_tcp_t _socket;
};