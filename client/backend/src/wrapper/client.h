#pragma once

#include <napi.h>
#include <memory>

#include "../core/client.h"

namespace quesync {
namespace client {
namespace wrapper {
class client : public Napi::ObjectWrap<client> {
   public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    client(const Napi::CallbackInfo &info);
    ~client();

    Napi::Value connect(const Napi::CallbackInfo &info);
    Napi::Value register_event_handler(const Napi::CallbackInfo &info);

    Napi::Value generate_errors_object(const Napi::CallbackInfo &info);

    template <typename T, Napi::Reference<Napi::Value> client::*member_ptr>
    Napi::Value module_get(const Napi::CallbackInfo &info) {
        // Create the object if not exists
        if ((this->*member_ptr).IsEmpty()) {
            Napi::Object val = T::create_instance(info.Env(), this->Value());

            // Create a reference to the object and suppress it's destruction
            this->*member_ptr = Napi::Reference<Napi::Value>::New(val, 1);
            (this->*member_ptr).SuppressDestruct();
        }

        return (this->*member_ptr).Value();
    }

    std::shared_ptr<quesync::client::client> core();

   private:
    inline static Napi::FunctionReference constructor;

    std::shared_ptr<quesync::client::client> _client;

    Napi::Reference<Napi::Value> _auth;
    Napi::Reference<Napi::Value> _users;
    Napi::Reference<Napi::Value> _channels;
    Napi::Reference<Napi::Value> _messages;
    Napi::Reference<Napi::Value> _voice;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync