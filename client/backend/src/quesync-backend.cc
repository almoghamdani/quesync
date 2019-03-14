#include <napi.h>
#include <uv.h>

#include "client.hpp"
#include "../../../shared/errors.h"
#include "../../../shared/utils.h"

void InitQuesyncError(Napi::Env env, Napi::Object exports)
{
    Napi::Object errors_object = Napi::Object::New(env);
    int error = SUCCESS;
    std::string error_name = "";

    do
    {
        // Convert the error to string and get it's name
        error_name = Utils::QuesyncErrorToString((QuesyncError)error);

        // If the error is found
        if (error_name != "")
        {
            // Set the error and it's name in the object of errors
            errors_object.Set(error_name, Napi::Number::New(env, error));
        }

        // Continue to the next error
        error++;
    } while (error_name != ""); // While we didn't reach an error that is not documented, continue checking for error names

    // Set the errors object in the exports for the frontend JavaScript
    exports.Set("errors", errors_object);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Client::Init(env, exports);
    InitQuesyncError(env, exports);
    return exports;
}

NODE_API_MODULE(quesync, Init)