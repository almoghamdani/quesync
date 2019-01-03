#pragma once
#include <nan.h>

#include "include/socket-manager.hpp"
#include "include/voice-chat.hpp"

class Client : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        Client();

        static void connect(const Nan::FunctionCallbackInfo<v8::Value>& info);

    private:
        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static Nan::Persistent<v8::Function> constructor;

        VoiceChat *_voiceChatManager;
};