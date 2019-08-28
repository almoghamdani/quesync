#include "client.h"

#ifdef QUESYNC_CLIENT
Napi::Object quesync::utils::client::json_to_object(Napi::Env env, nlohmann::json json) {
    // Convert the json dump to a Napi string object
    Napi::String json_dump = Napi::String::New(env, json.dump());

    // Get it's value
    napi_value json_value = json_dump.operator napi_value();

    // Get the JavaScript json parse function
    Napi::Object json_obj = Napi::Object(env, env.Global().Get("JSON"));
    Napi::Function parse = Napi::Function(env, json_obj.Get("parse"));

    // Parse the json dump to a JavaScript Object using the function
    return Napi::Object(env, parse.Call(1, (napi_value *)&json_value));
}
#endif