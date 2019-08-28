#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

#include "../../../../shared/utils/client.h"

namespace quesync {
namespace client {
namespace wrapper {
class auth : public Napi::ObjectWrap<auth>, public module<auth> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(
            env, "Auth",
            {InstanceMethod("login", &auth::login), InstanceMethod("register", &auth::signup),
             InstanceMethod("sessionAuth", &auth::session_auth),
             InstanceMethod("getUser", &auth::get_user)});
    }

    auth(const Napi::CallbackInfo &info) : Napi::ObjectWrap<auth>(info), module(info) {}

    Napi::Value login(const Napi::CallbackInfo &info) {
        std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, username, password]() {
            std::string session_id;

            std::shared_ptr<quesync::user> user;

            // Login and get session id
            session_id = _client->core()->auth()->login(username, password);

            // Get the user
            user = _client->core()->auth()->get_user();

            return nlohmann::json{{"user", *user}, {"sessionId", session_id}};
        });
    }

    Napi::Value signup(const Napi::CallbackInfo &info) {
        std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>(),
                    email = info[2].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, username, password, email]() {
            std::string session_id;

            std::shared_ptr<quesync::user> user;

            // Sign up and get session id
            session_id = _client->core()->auth()->signup(username, password, email);

            // Get the user
            user = _client->core()->auth()->get_user();

            return nlohmann::json{{"user", *user}, {"sessionId", session_id}};
        });
    }

    Napi::Value session_auth(const Napi::CallbackInfo &info) {
        std::string session_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, session_id]() {
            std::shared_ptr<quesync::user> user;

            // Authenticate using session id
            _client->core()->auth()->session_auth(session_id);

            // Get the user
            user = _client->core()->auth()->get_user();

            return nlohmann::json{{"user", *user}, {"sessionId", session_id}};
        });
    }

    Napi::Value get_user(const Napi::CallbackInfo &info) {
        std::shared_ptr<quesync::user> user;

        // Get the user
        user = _client->core()->auth()->get_user();

        // Convert the user to object if not null
        return user ? utils::client::json_to_object(info.Env(), *user) : info.Env().Null();
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync