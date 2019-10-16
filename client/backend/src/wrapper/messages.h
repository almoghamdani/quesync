#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

namespace quesync {
namespace client {
namespace wrapper {
class messages : public Napi::ObjectWrap<messages>, public module<messages> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(env, "Messages",
                           {InstanceMethod("sendMessage", &messages::send_message),
                            InstanceMethod("getChannelMessages", &messages::get_channel_messages)});
    }

    messages(const Napi::CallbackInfo &info) : Napi::ObjectWrap<messages>(info), module(info) {}

    Napi::Value send_message(const Napi::CallbackInfo &info) {
        std::string content = info[0].As<Napi::String>(), channel_id = info[1].As<Napi::String>();

        std::string attachment_id = info.Length() != 3 ? "" : (std::string)info[2].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, content, channel_id, attachment_id]() {
            std::shared_ptr<quesync::message> message;

            // Send the message to the channel
            message = _client->core()->messages()->send_message(content, channel_id, attachment_id);

            return nlohmann::json{{"message", *message}};
        });
    }

    Napi::Value get_channel_messages(const Napi::CallbackInfo &info) {
        std::string channel_id = info[0].As<Napi::String>();
        unsigned int amount = info[1].As<Napi::Number>(), offset = info[2].As<Napi::Number>();

        return executer::create_executer(info.Env(), [this, channel_id, amount, offset]() {
            std::vector<quesync::message> messages;

            // Get the messages of the channel
            messages =
                _client->core()->messages()->get_channel_messages(channel_id, amount, offset);

            return nlohmann::json{{"messages", messages}, {"channelId", channel_id}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync