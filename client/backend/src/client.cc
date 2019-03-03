#include "client.hpp"

#include "socket-manager.hpp"
#include "../../../shared/utils.h"
#include "../../../shared/packets/login_packet.h"
#include "../../../shared/packets/register_packet.h"
#include "../../../shared/packets/error_packet.h"
#include "../../../shared/packets/search_packet.h"
#include "../../../shared/packets/friend_request_packet.h"

Napi::FunctionReference Client::constructor;

Client::Client(const Napi::CallbackInfo &info) : 
    Napi::ObjectWrap<Client>(info), 
    _socket(SocketManager::io_context),
    _user(nullptr)
{
    Napi::Env env = info.Env();
}

void Client::connect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    tcp::endpoint server_endpoint;

    // Convert first parameter to string
    std::string ip = info[0].As<Napi::String>();

    // Get the endpoint of the server to connect to
    SocketManager::GetEndpoint(ip.c_str(), SERVER_PORT, server_endpoint);

    // Create the voice chat manager and start a communication with the server
    //_voiceChatManager = new VoiceChat((const char *)*ip);

    try {
        // Try to connect to the server
        _socket.connect(server_endpoint);
    } catch (std::system_error& ex)
    {
        // Throw error on excpetion
        throw Napi::Error::New(env, std::to_string(ex.code().value()));
    }
}

Napi::Value Client::login(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    ResponsePacket *response_packet;
    Napi::Object res = Napi::Object::New(env);

    // Convert parameters to satring
    std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>();

    // Create a login packet from the username and password
    LoginPacket login_packet(username, password);

    // If already authenticated, return error
    if (_user)
    {
        res["error"] = Napi::Number::New(env, ALREADY_AUTHENTICATED);

        return res;
    }

    // Copy the login packet to the dat buffer
    Utils::CopyString(login_packet.encode(), _data);

    // Send the server the login packet
    _socket.write_some(asio::buffer(_data, strlen(_data)));

    // Get a response from the server
    _socket.read_some(asio::buffer(_data, MAX_DATA_LEN));

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    // If the response is an error, handle the error
    if (response_packet && response_packet->type() == ERROR_PACKET)
    {
        // Set the error code from the response packet
        res["error"] = Napi::Number::New(env, ((ErrorPacket *)response_packet)->error());

        // Set no user since an error occurred
        res["user"] = env.Null();
    } else if (response_packet && response_packet->type() == AUTHENTICATED_PACKET) // If the response packet is a valid authentication response, get the user from it
    {
        // Create a new user
        _user = new User();

        // Deserialize the user data from the response data
        _user->deserialize(response_packet->data());

        // Set success error code
        res["error"] = Napi::Number::New(env, SUCCESS);

        // Return the user serialized
        res["user"] = Utils::JsonToObject(env, _user->json());
    }

    return res;
}

Napi::Value Client::signup(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    ResponsePacket *response_packet;
    Napi::Object res = Napi::Object::New(env);

    // Convert parameters to string
    std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>(),
                email = info[2].As<Napi::String>(), nickname = info[3].As<Napi::String>();

    // Create a login packet from the username and password
    RegisterPacket register_packet(username, password, email, nickname);

    // If already authenticated, return error
    if (_user)
    {
        res["error"] = Napi::Number::New(env, ALREADY_AUTHENTICATED);

        return res;
    }

    // Copy the register packet to the data buffer
    Utils::CopyString(register_packet.encode(), _data);

    // Send the server the register packet
    _socket.write_some(asio::buffer(_data, strlen(_data)));

    // Get a response from the server
    _socket.read_some(asio::buffer(_data, MAX_DATA_LEN));

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    // If the response is an error, handle the error
    if (response_packet && response_packet->type() == ERROR_PACKET)
    {
        // Set the error code from the response packet
        res["error"] = Napi::Number::New(env, ((ErrorPacket *)response_packet)->error());

        // Set no user since an error occurred
        res["user"] = env.Null();
    } else if (response_packet && response_packet->type() == AUTHENTICATED_PACKET) // If the response packet is a valid authentication response, get the user from it
    {
        // Create a new user
        _user = new User();

        // Deserialize the user data from the response data
        _user->deserialize(response_packet->data());

        // Set success error code
        res["error"] = Napi::Number::New(env, SUCCESS);

        // Return the user serialized
        res["user"] = Utils::JsonToObject(env, _user->json());
    }

    return res;
}

Napi::Value Client::search(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    ResponsePacket *response_packet;
    Napi::Object res = Napi::Object::New(env);

    // Convert parameters to string
    std::string nickname = info[0].As<Napi::String>();
    int tag = info[1].IsUndefined() ? -1 : info[0].As<Napi::Number>();

    SearchPacket search_packet(nickname, tag);

    // If not authenticated, return error
    if (!_user)
    {
        res["error"] = Napi::Number::New(env, NOT_AUTHENTICATED);

        return res;
    }

    // Copy the search packet to the data buffer
    Utils::CopyString(search_packet.encode(), _data);

    // Send the server the search packet
    _socket.write_some(asio::buffer(_data, strlen(_data)));

    // Get a response from the server
    _socket.read_some(asio::buffer(_data, MAX_DATA_LEN));

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    // If the response is an error, handle the error
    if (response_packet && response_packet->type() == ERROR_PACKET)
    {
        // Set the error code from the response packet
        res["error"] = Napi::Number::New(env, ((ErrorPacket *)response_packet)->error());

        // Set no search results since an error occurred
        res["search_results"] = env.Null();
    }
    // If the response packet is a valid search results response, get the user from it
    else if (response_packet && response_packet->type() == SEARCH_RESULTS_PACKET)
    {       
        // Set success error code
        res["error"] = Napi::Number::New(env, SUCCESS);

        // Return the search results in an object
        res["search_results"] = Utils::JsonToObject(env, nlohmann::json::parse(response_packet->data()));
    }

    return res;
}

Napi::Value Client::sendFriendRequest(const Napi::CallbackInfo& info)
{
Napi::Env env = info.Env();

    ResponsePacket *response_packet;
    Napi::Object res = Napi::Object::New(env);

    // Convert parameters to string
    std::string friend_id = info[0].As<Napi::String>();

    FriendRequestPacket friend_request_packet(friend_id);

    // If not authenticated, return error
    if (!_user)
    {
        res["error"] = Napi::Number::New(env, NOT_AUTHENTICATED);

        return res;
    }

    // Copy the friend request packet to the data buffer
    Utils::CopyString(friend_request_packet.encode(), _data);

    // Send the server the friend request packet
    _socket.write_some(asio::buffer(_data, strlen(_data)));

    // Get a response from the server
    _socket.read_some(asio::buffer(_data, MAX_DATA_LEN));

    // Parse the response packet
    response_packet = (ResponsePacket *)Utils::ParsePacket(_data);

    // If the response is an error, handle the error
    if (response_packet && response_packet->type() == ERROR_PACKET)
    {
        // Set the error code from the response packet
        res["error"] = Napi::Number::New(env, ((ErrorPacket *)response_packet)->error());
    }
    // If the response packet is a friend request confirmation response, get the user from it
    else if (response_packet && response_packet->type() == FRIEND_REQUEST_SENT_PACKET)
    {       
        // Set success error code
        res["error"] = Napi::Number::New(env, SUCCESS);
    }

    return res;
}

Napi::Object Client::Init(Napi::Env env, Napi::Object exports) {
    // Create scope for Client object
    Napi::HandleScope scope(env);

    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "Client", {
        InstanceMethod("connect", &Client::connect),
        InstanceMethod("login", &Client::login),
        InstanceMethod("register", &Client::signup),
        InstanceMethod("search", &Client::search),
        InstanceMethod("sendFriendRequest", &Client::sendFriendRequest)
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