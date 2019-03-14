#include "executer.hpp"

#include "../../../shared/utils.h"

// This function will be the fake callback to be fed into the NAPI api
static Napi::Value fake_callback(Napi::CallbackInfo const &info)
{
    return info.Env().Undefined();
}

// This var will hold the fake callback
static Napi::Reference<Napi::Function> _fake_callback;

// This function is used to construct the fake callback and get it
static const Napi::Reference<Napi::Function> &get_fake_callback(Napi::Env const &env)
{
    // If the fake callback is empty, create it
    if (_fake_callback.IsEmpty())
    {
        _fake_callback = Napi::Reference<Napi::Function>::New(Napi::Function::New(env, fake_callback), 1);
    }

    return _fake_callback;
}

Executer::Executer(std::function<nlohmann::json()> func, const Napi::Promise::Deferred &deferred) : Napi::AsyncWorker(get_fake_callback(deferred.Env()).Value()), // Init the AsyncWorker with a fake callback
                                                                                                    _func(func),
                                                                                                    _deferred(deferred)
{
}

Executer::~Executer()
{
}

void Executer::Execute()
{
    // Call the function and get the return data from it
    _return_data = _func();
}

void Executer::OnOK()
{
    // If no error occurred, resolve the promise
    if (_return_data["error"] == 0)
    {
        // Convert the return data json to an object and send it back to the frontend
        _deferred.Resolve(Utils::JsonToObject(Env(), _return_data));
    }
    else
    {
        // Convert the return data json to an object and send it back to the frontend
        _deferred.Reject(Utils::JsonToObject(Env(), _return_data));
    }
}

void Executer::OnError(const Napi::Error &e)
{
    // We shouldn't get here since the errors are caught in the execute function
}