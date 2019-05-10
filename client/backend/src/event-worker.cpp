#include "event-worker.h"

#include "../../../shared/utils.h"

EventWorker::EventWorker(Napi::Function event_handler, nlohmann::json &event_data) : Napi::AsyncWorker(event_handler),
                                                                                     _event_data(event_data)
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
    // Convert the event data to Object and call the callback function with it
    Callback().Call({Utils::JsonToObject(Env(), _event_data)});
}

void EventWorker::OnError(const Napi::Error &e)
{
}