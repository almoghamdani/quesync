#pragma once
#include <nan.h>

#include "include/socket-manager.hpp"

class Client : public Nan::ObjectWrap {
    public:
        Client();
        static void Init(v8::Local<v8::Object> exports);

    private:
        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static Nan::Persistent<v8::Function> constructor;
};