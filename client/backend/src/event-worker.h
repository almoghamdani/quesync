#pragma once
#include <napi.h>

#include <nlohmann/json.hpp>

class EventWorker : public Napi::AsyncWorker
{
public:
    EventWorker(Napi::Function callback);
    ~EventWorker();

    virtual void Execute();
    virtual void OnOK();
    virtual void OnError(const Napi::Error &e);

    void setEventData(nlohmann::json &event_data);

private:
    nlohmann::json _event_data;
};