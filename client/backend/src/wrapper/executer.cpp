#include "executer.h"

#include "../../../shared/exception.h"
#include "../../../shared/utils/client.h"

Napi::Promise quesync::client::wrapper::executer::create_executer(
    Napi::Env env, std::function<nlohmann::json()> func) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    // Create a new executer worker that will execute the function in the background
    executer *e = new executer(func, deferred);

    // Queue the executer worker
    e->Queue();

    // Return the promise
    return deferred.Promise();
}

quesync::client::wrapper::executer::executer(std::function<nlohmann::json()> func,
                                             const Napi::Promise::Deferred &deferred)
    : Napi::AsyncWorker(deferred.Env()), _func(func), _deferred(deferred) {}

quesync::client::wrapper::executer::~executer() {}

void quesync::client::wrapper::executer::Execute() {
    // Call the function and get the return data from it
    try {
        _return_data = _func();
    } catch (exception &ex) {
        // Set error if exception happend
        SetError(std::to_string((int)ex.error_code()));
    } catch (...) {
        // Set unknown error
        SetError(std::to_string((int)error::unknown_error));
    }
}

void quesync::client::wrapper::executer::OnOK() {
    // Set error as success
    _return_data["error"] = error::success;

    // Convert the return data json to an object and send it back to the frontend
    _deferred.Resolve(utils::client::json_to_object(Env(), _return_data));
}

void quesync::client::wrapper::executer::OnError(const Napi::Error &e) {
    // Get the error code and return in in an object
    _deferred.Reject(
        utils::client::json_to_object(Env(), nlohmann::json{{"error", std::stoi(e.Message())}}));
}