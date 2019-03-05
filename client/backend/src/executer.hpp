#include <napi.h>

#include <nlohmann/json.hpp>

class Executer : public Napi::AsyncWorker
{
public:
    Executer(std::function<nlohmann::json()> func, const Napi::Promise::Deferred& deferred);
    ~Executer();

    virtual void Execute();
    virtual void OnOK();
    virtual void OnError(const Napi::Error& e);

private:
    std::function<nlohmann::json()> _func;
    nlohmann::json _return_data;

    Napi::Promise::Deferred _deferred;
};