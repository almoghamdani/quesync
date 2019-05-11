#include "event-worker.h"

#include "../../../shared/utils.h"

EventWorker::EventWorker(Napi::Function event_handler) : Napi::AsyncWorker(event_handler)
{
}

EventWorker::~EventWorker()
{
}

void EventWorker::Execute()
{
}

void EventWorker::OnOK()
{
    Napi::HandleScope scope(Env());

    // Convert the event data to Object and call the callback function with it
    Callback().Call({Utils::JsonToObject(Env(), _event_data)});
}

void EventWorker::OnError(const Napi::Error &e)
{
}

void EventWorker::setEventData(nlohmann::json &event_data)
{
    _event_data = event_data;
}