#include "client.hpp"

#include "socket-manager.hpp"
#include "../../../shared/utils.h"
#include "../../../shared/packets/login_packet.h"
#include "../../../shared/packets/error_packet.h"

Napi::FunctionReference Client::constructor;

Client::Client(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Client>(info), _socket(SocketManager::io_context)
{
    Napi::Env env = info.Env();
}

void Client::connect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    tcp::endpoint serverEndpoint;

    // Convert first parameter to string
    std::string ip = info[0].As<Napi::String>();

    // If the first parameter which should be the IP address is undefined send error
    if (info[0].IsUndefined())
    {
        throw Napi::Error::New(env, "Missing IP address parameter!");
    }

    // Get the endpoint of the server to connect to
    SocketManager::GetEndpoint(ip.c_str(), SERVER_PORT, serverEndpoint);

    // Create the voice chat manager and start a communication with the server
    //_voiceChatManager = new VoiceChat((const char *)*ip);

    try {
        // Try to connect to the server
        _socket.connect(serverEndpoint);
    } catch (std::system_error& ex)
    {
        // Format the error message
        std::string error = (std::stringstream() << ex.code().message() << " [" << ex.code() << "]").str();

        // Throw error on excpetion
        throw Napi::Error::New(env, error);
    }
}

Napi::Value Client::login(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    Packet *responsePacket;
    Napi::Object res = Napi::Object::New(env);

    // Convert parameters to string
    std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>();

    // Create a login packet from the username and password
    LoginPacket loginPacket(username, password);

    // Check parameters
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString())
    {
        throw Napi::Error::New(env, "Missing parameters!");
    }

    // Copy the login packet to the data buffer
    Utils::CopyString(loginPacket.encode(), _data);

    // Send the server the login packet
    _socket.write_some(asio::buffer(_data, strlen(_data)));

    // Get a response from the server
    _socket.read_some(asio::buffer(_data, MAX_DATA_LEN));

    // Parse the response packet
    responsePacket = Utils::ParsePacket(_data);

    // If the response is an error, handle the error
    if (responsePacket && responsePacket->type() == ERROR_PACKET)
    {
        // Set the error code from the response packet
        res["error"] = Napi::Number::New(env, ((ErrorPacket *)responsePacket)->error());

        // Set no user since an error occurred
        res["user"] = env.Null();
    }

    return res;
}

Napi::Object Client::Init(Napi::Env env, Napi::Object exports) {
    // Create scope for Client object
    Napi::HandleScope scope(env);

    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "Client", {
        InstanceMethod("connect", &Client::connect),
        InstanceMethod("login", &Client::login)
    });

    // Create a peristent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    constructor = Napi::Persistent(func);

    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    constructor.SuppressDestruct();

    exports.Set("Client", func);
    return exports;
}