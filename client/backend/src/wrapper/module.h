#pragma once

#include <napi.h>

namespace quesync {
namespace client {
namespace wrapper {
class client;

template <class T>
class module {
   public:
    static Napi::Object create_instance(Napi::Env env, Napi::Object client) {
        // If the constructor is empty, init it
        if (T::constructor.IsEmpty()) {
            T::constructor = Napi::Persistent(T::create_constructor_function(env));
            T::constructor.SuppressDestruct();
        }

        return T::constructor.New({client});
    };

    module(const Napi::CallbackInfo &info)
        : _client(Napi::ObjectWrap<client>::Unwrap(info[0].As<Napi::Object>())) {}

   protected:
    client *_client;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync