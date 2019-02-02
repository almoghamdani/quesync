#include "include/client.hpp"

Nan::Persistent<v8::Function> Client::constructor;

Client::Client()
{
  // Initiate the socket manager
  //SocketManager::InitSocketManager();
}

void Client::connect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  // Get the object
  Client* obj = ObjectWrap::Unwrap<Client>(info.Holder());

  // Convert first parameter to string
  Nan::Utf8String ip(info[0]);

  // If the first parameter which should be the IP address is undefined send error
  if (info[0]->IsUndefined())
  {
    Nan::ThrowError(Nan::Error("Missing IP address parameter!"));
  }

  // Try to create a tcp socket with the server
  try {
    //SocketManager::CreateTCPSocket(&obj->_socket, (const char *)*ip, SERVER_PORT);
  } catch (std::exception ex)
  {
    // Throw error on excpetion
    Nan::ThrowError(Nan::Error(ex.what()));
  }
}

void Client::login(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  quesync_packet_t login_packet;

  // Get the object
  Client* obj = ObjectWrap::Unwrap<Client>(info.Holder());

  // Convert parameters to string
  Nan::Utf8String email(info[0]), password(info[1]);

  // If one of the parameters is undefined, send error
  if (info[0]->IsUndefined() || info[1]->IsUndefined())
  {
    Nan::ThrowError(Nan::Error("Missing parameters!"));
  }

  // Set login packet as a login request + Set data as the email and password
  login_packet.type = LOGIN_REQUEST;
  sprintf(login_packet.data, "%s;;;%s", (const char *)*email, (const char *)*password);

  // Send to the server the login request
  //SocketManager::Send(&obj->_socket, &login_packet, sizeof(login_packet));
}

void Client::Init(v8::Local<v8::Object> exports) {
  // Create scope for client
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Client").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "connect", connect);
  Nan::SetPrototypeMethod(tpl, "login", login);

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