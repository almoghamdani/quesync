#include "include\client.hpp"

Nan::Persistent<v8::Function> Client::constructor;

void Client::Init(v8::Local<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Client").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SocketManager::InitSocketManager();

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