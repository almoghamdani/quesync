#include "event-handler.h"

#include <stdexcept>

void EventHandler::callEvent(std::string &event_name, nlohmann::json &event_data)
{
    try {
        // Call the event handler function for the event with the event data
        _event_handlers[event_name](event_data);
    } catch(...) {
        throw std::runtime_error("No event handler found for '" + event_name + "'");
    }
}

void EventHandler::registerEventHandler(std::string& event_name, std::function<void(nlohmann::json&)> event_handler)
{
    // Set the event handler for the event
    _event_handlers[event_name] = event_handler;
}