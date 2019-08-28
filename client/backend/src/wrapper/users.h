#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

namespace quesync {
namespace client {
namespace wrapper {
class users : public Napi::ObjectWrap<users>, public module<users> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(env, "Users",
                           {InstanceMethod("getUserProfile", &users::get_user_profile),
                            InstanceMethod("search", &users::search),
                            InstanceMethod("sendFriendRequest", &users::send_friend_request),
                            InstanceMethod("setFriendshipStatus", &users::set_friendship_status)});
    }

    users(const Napi::CallbackInfo &info) : Napi::ObjectWrap<users>(info), module(info) {}

    Napi::Value get_user_profile(const Napi::CallbackInfo &info) {
        std::string user_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, user_id]() {
            std::shared_ptr<quesync::profile> profile;

            // Get the user's profile
            profile = _client->core()->users()->get_user_profile(user_id);

            return nlohmann::json{{"profile", *profile}};
        });
    }

    Napi::Value search(const Napi::CallbackInfo &info) {
        std::string nickname = info[0].As<Napi::String>();
        int tag = info[1].IsUndefined() ? -1 : info[1].As<Napi::Number>();

        return executer::create_executer(info.Env(), [this, nickname, tag]() {
            std::vector<quesync::profile> search_results;

            // Search by the username and tag
            search_results = _client->core()->users()->search(nickname, tag);

            return nlohmann::json{{"searchResults", search_results}};
        });
    }

    Napi::Value send_friend_request(const Napi::CallbackInfo &info) {
        std::string friend_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, friend_id]() {
            std::shared_ptr<quesync::friend_request> friend_request;

            // Send the friend request to the server
            friend_request = _client->core()->users()->send_friend_request(friend_id);

            return nlohmann::json{{"friendRequest", *friend_request}};
        });
    }

    Napi::Value set_friendship_status(const Napi::CallbackInfo &info) {
        std::string friend_id = info[0].As<Napi::String>();
        bool status = info[1].As<Napi::Boolean>();

        return executer::create_executer(info.Env(), [this, friend_id, status]() {
            // Set the friendship status
            _client->core()->users()->set_friendship_status(friend_id, status);

            return nlohmann::json{{"friendId", friend_id}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync