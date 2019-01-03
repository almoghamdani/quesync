#include "include\client.hpp"

Nan::Persistent<v8::Function> Client::constructor;

Client::Client()
{
  // Initiate the socket manager
  SocketManager::InitSocketManager();
}

void Client::connect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Nan::Utf8String ip(info[0]);

  // If the first parameter which should be the IP address is undefined send error
  if (info[0]->IsUndefined())
  {
    Nan::ThrowError(Nan::Error("Missing IP address parameter!"));
  }

  // Create a voice chat manager just as an example
  VoiceChat *voiceChatManager = new VoiceChat((const char *)*ip);
}

void Client::Init(v8::Local<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Client").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "connect", connect);

  // Set the constructor function + Export it as a function for js
  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Client").ToLocalChecked(), tpl->GetFunction());
}

void Client::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new Client(...)`
    Client* obj = new Client();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Return the object's constructor function
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(Nan::NewInstance(cons).ToLocalChecked());
  }
}