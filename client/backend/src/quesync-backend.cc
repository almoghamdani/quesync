#include <napi.h>
#include <uv.h>

#include "client.hpp"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Client::Init(env, exports);
    return exports;
}

NODE_API_MODULE(quesync, Init)