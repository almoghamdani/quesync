#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

namespace quesync {
namespace client {
namespace wrapper {
class channels : public Napi::ObjectWrap<channels>, public module<channels> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(env, "Channels",
                           {InstanceMethod("getPrivateChannel", &channels::get_private_channel)});
    }

    channels(const Napi::CallbackInfo &info) : Napi::ObjectWrap<channels>(info), module(info) {}

    Napi::Value get_private_channel(const Napi::CallbackInfo &info) {
        std::string user_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, user_id]() {
            std::shared_ptr<quesync::channel> channel;

            // Get the private channel with the user
            channel = _client->core()->channels()->get_private_channel(user_id);

            return nlohmann::json{{"channel", *channel}, {"userId", user_id}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync