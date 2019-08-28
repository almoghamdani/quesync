#include <napi.h>
#include <uv.h>

#include "wrapper/client.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    // Update the exports with the client class
    quesync::client::wrapper::client::init(env, exports);

    return exports;
}

NODE_API_MODULE(quesync, Init)