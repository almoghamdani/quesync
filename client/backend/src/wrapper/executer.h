#pragma once
#include <napi.h>

#include <nlohmann/json.hpp>

namespace quesync {
namespace client {
namespace wrapper {
class executer : public Napi::AsyncWorker {
   public:
    static Napi::Promise create_executer(Napi::Env env, std::function<nlohmann::json()> func);

    executer(std::function<nlohmann::json()> func, const Napi::Promise::Deferred &deferred);
    ~executer();

    virtual void Execute();
    virtual void OnOK();
    virtual void OnError(const Napi::Error &e);

   private:
    std::function<nlohmann::json()> _func;
    nlohmann::json _return_data;

    Napi::Promise::Deferred _deferred;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync
