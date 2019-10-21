#include "client.h"

#include <magic_enum.hpp>

#include "auth.h"
#include "channels.h"
#include "event_names.h"
#include "executer.h"
#include "messages.h"
#include "users.h"
#include "voice.h"
#include "files.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/utils/client.h"

Napi::Object quesync::client::wrapper::client::init(Napi::Env env, Napi::Object exports) {
    // Define the class and get it's ctor function
    Napi::Function ctor_func =
        DefineClass(env, "Client",
                    {InstanceMethod("connect", &client::connect),
                     InstanceMethod("registerEventHandler", &client::register_event_handler),
                     InstanceMethod("clearAllEventHandlers", &client::clear_all_event_handlers),
                     InstanceMethod("auth", &client::module_get<auth, &client::_auth>),
                     InstanceMethod("users", &client::module_get<users, &client::_users>),
                     InstanceMethod("channels", &client::module_get<channels, &client::_channels>),
                     InstanceMethod("messages", &client::module_get<messages, &client::_messages>),
                     InstanceMethod("voice", &client::module_get<voice, &client::_voice>),
                     InstanceMethod("files", &client::module_get<files, &client::_files>),
                     InstanceMethod("errors", &client::generate_errors_object)});

    // Set the class's ctor function as a persistent object to keep it in memory
    constructor = Napi::Persistent(ctor_func);
    constructor.SuppressDestruct();

    // Export the ctor
    exports.Set("Client", ctor_func);
    return exports;
}

quesync::client::wrapper::client::client(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<client>(info), _client(std::make_shared<quesync::client::client>()) {
    // Initialize the client
    _client->init();
}

Napi::Value quesync::client::wrapper::client::connect(const Napi::CallbackInfo &info) {
    std::string ip = info[0].As<Napi::String>();

    return executer::create_executer(info.Env(), [this, ip]() {
        // Try to connect to the server by the given IP
        _client->connect(ip);

        return nlohmann::json();
    });
}

Napi::Value quesync::client::wrapper::client::register_event_handler(
    const Napi::CallbackInfo &info) {
    std::string event_name = info[0].As<Napi::String>();
    Napi::Function event_handle_function = info[1].As<Napi::Function>();

    Napi::ThreadSafeFunction tsfn;

    event_type type;

    try {
        // Try to find the event type
        type = std::find_if(event_names.begin(), event_names.end(),
                            [event_name](const std::pair<event_type, std::string> &p) {
                                return p.second == event_name;
                            })
                   ->first;
    } catch (...) {
        throw exception(error::invalid_event);
    }

    // Create the thread safe function object
    tsfn = Napi::ThreadSafeFunction::New(info.Env(), event_handle_function, event_name, 0, 1,
                                         [](Napi::Env) {});

    // Register the event handler
    _client->communicator()->event_handler().register_event_handler(
        type, [tsfn_ptr = std::make_shared<Napi::ThreadSafeFunction>(std::move(tsfn))](
                  std::shared_ptr<event> evt) {
            // Allocate memory for the json to be sent to the javascript function
            nlohmann::json *j = new nlohmann::json(evt->encode());

            // Call the javascript event handler async with the json data
            tsfn_ptr.get()->NonBlockingCall(
                j, [](Napi::Env env, Napi::Function jsCallback, nlohmann::json *j) {
                    // Call the callback javascript function with the json
                    jsCallback.Call({utils::client::json_to_object(env, *j)});

                    // Free the json memory
                    delete j;
                });
        });

    return info.Env().Undefined();
}

Napi::Value quesync::client::wrapper::client::clear_all_event_handlers(
    const Napi::CallbackInfo &info) {
    // Clear all event handlers
    _client->communicator()->event_handler().clear_all_event_handlers();

    return info.Env().Undefined();
}

Napi::Value quesync::client::wrapper::client::generate_errors_object(
    const Napi::CallbackInfo &info) {
    // Get all error_names
    constexpr auto error_names = magic_enum::enum_names<quesync::error>();

    Napi::Object errors_object = Napi::Object::New(info.Env());

    // Add each error to the object
    for (int i = 0; i < error_names.size(); i++) {
        errors_object.Set((std::string)error_names[i], i);
    }

    return errors_object;
}

std::shared_ptr<quesync::client::client> quesync::client::wrapper::client::core() {
    return _client;
}