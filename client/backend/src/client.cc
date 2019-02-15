#include "client.hpp"

#include "socket-manager.hpp"
#include "../../../shared/packets/login_packet.h"

Nan::Persistent<v8::Function> Client::constructor;

Client::Client() : _socket(SocketManager::io_context)
{
}

void Client::connect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    tcp::endpoint serverEndpoint;

    // Get the object
    Client* obj = ObjectWrap::Unwrap<Client>(info.Holder());

    // Convert first parameter to string
    Nan::Utf8String ip(info[0]);

    // If the first parameter which should be the IP address is undefined send error
    if (info[0]->IsUndefined())
    {
        Nan::ThrowError(Nan::Error("Missing IP address parameter!"));
    }

    // Get the endpoint of the server to connect to
    SocketManager::GetEndpoint((const char *)*ip, SERVER_PORT, serverEndpoint);

    // Create the voice chat manager and start a communication with the server
    //obj->_voiceChatManager = new VoiceChat((const char *)*ip);

    try {
        // Try to connect to the server
        obj->_socket.connect(serverEndpoint);
    } catch (std::system_error& ex)
    {
        // Format the error message
        std::string error = (std::stringstream() << ex.code().message() << " [" << ex.code() << "]").str();

        // Throw error on excpetion
        Nan::ThrowError(Nan::Error(error.c_str()));
    }
}

void Client::login(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    // Get the object
    Client* obj = ObjectWrap::Unwrap<Client>(info.Holder());

    // Convert parameters to string
    Nan::Utf8String username(info[0]), password(info[1]);

    // Create a login packet from the username and password
    LoginPacket loginPacket((const char *)*username, (const char *)*password);

    // If one of the parameters is undefined, send error
    if (info[0]->IsUndefined() || info[1]->IsUndefined())
    {
        Nan::ThrowError(Nan::Error("Missing parameters!"));
    }

    // Send the server the login packet
    obj->_socket.write_some(asio::buffer(loginPacket.encode(), loginPacket.encode().length()));
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