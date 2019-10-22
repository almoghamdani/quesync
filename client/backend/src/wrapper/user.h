#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

#include "../../../../shared/utils/client.h"

namespace quesync {
namespace client {
namespace wrapper {
class user : public Napi::ObjectWrap<user>, public module<user> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(
            env, "User",
            {InstanceMethod("setProfilePhoto", &user::set_profile_photo)});
    }

    user(const Napi::CallbackInfo &info) : Napi::ObjectWrap<user>(info), module(info) {}

    Napi::Value set_profile_photo(const Napi::CallbackInfo &info) {
        std::string file_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, file_id]() {
            // Set the profile photo for the user
			_client->core()->user()->set_profile_photo(file_id);

            return nlohmann::json{{"fileId", file_id}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync