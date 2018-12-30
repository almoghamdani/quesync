#include <nan.h>
#include "include/socket-manager.hpp"
#include "include/voice-chat.hpp"

void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  VoiceChat *voiceChatManager;

  SocketManager::InitSocketManager();
  voiceChatManager = new VoiceChat("127.0.0.1");

  info.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("hello").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Method)->GetFunction());
}

NODE_MODULE(hello, Init)