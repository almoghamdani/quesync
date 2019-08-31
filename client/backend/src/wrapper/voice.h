#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

namespace quesync {
namespace client {
namespace wrapper {
class voice : public Napi::ObjectWrap<voice>, public module<voice> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(
            env, "Voice",
            {InstanceMethod("call", &voice::call), InstanceMethod("joinCall", &voice::join_call),
             InstanceMethod("leaveCall", &voice::leave_call),
             InstanceMethod("setVoiceState", &voice::set_voice_state),
             InstanceMethod("getChannelCalls", &voice::get_channel_calls)});
    }

    voice(const Napi::CallbackInfo &info) : Napi::ObjectWrap<voice>(info), module(info) {}

    Napi::Value call(const Napi::CallbackInfo &info) {
        std::string channel_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, channel_id]() {
            std::shared_ptr<call_details> call_details;

            // Call the channel and get the voice states
            call_details = _client->core()->voice()->call(channel_id);

            return nlohmann::json{{"callDetails", *call_details}, {"channelId", channel_id}};
        });
    }

    Napi::Value join_call(const Napi::CallbackInfo &info) {
        std::string channel_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, channel_id]() {
            std::unordered_map<std::string, quesync::voice::state> voice_states;

            // Join the call in the channel and get the voice states
            voice_states = _client->core()->voice()->join_call(channel_id);

            return nlohmann::json{{"voiceStates", voice_states}, {"channelId", channel_id}};
        });
    }

    Napi::Value leave_call(const Napi::CallbackInfo &info) {
        return executer::create_executer(info.Env(), [this]() {
            // Leave the call
            _client->core()->voice()->leave_call();

            return nlohmann::json();
        });
    }

    Napi::Value set_voice_state(const Napi::CallbackInfo &info) {
        bool mute = info[0].As<Napi::Boolean>(), deafen = info[1].As<Napi::Boolean>();

        return executer::create_executer(info.Env(), [this, mute, deafen]() {
            std::shared_ptr<quesync::voice::state> voice_state;

            // Set the voice state
            voice_state = _client->core()->voice()->set_voice_state(mute, deafen);

            return nlohmann::json{{"voiceState", *voice_state}};
        });
    }

    Napi::Value get_channel_calls(const Napi::CallbackInfo &info) {
        std::string channel_id = info[0].As<Napi::String>();
        unsigned int amount = info[1].As<Napi::Number>(), offset = info[2].As<Napi::Number>();

        return executer::create_executer(info.Env(), [this, channel_id, amount, offset]() {
            std::vector<quesync::call> calls;

            // Get the calls of the channel
            calls =
                _client->core()->voice()->get_channel_calls(channel_id, amount, offset);

            return nlohmann::json{{"calls", calls}, {"channelId", channel_id}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync