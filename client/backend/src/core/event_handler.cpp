#include "event_handler.h"

#include <magic_enum.hpp>
#include <stdexcept>

void quesync::client::event_handler::call_event(std::shared_ptr<quesync::event> evt) {
    try {
        // Call the event handler function for the event with the event data
        _event_handlers[evt->type](evt);
    } catch (...) {
        throw std::runtime_error("No event handler found for '" +
                                 static_cast<std::string>(magic_enum::enum_name(evt->type)) + "'");
    }
}

void quesync::client::event_handler::register_event_handler(
    event_type type, std::function<void(std::shared_ptr<quesync::event>)> event_handler) {
    // Set the event handler for the event
    _event_handlers[type] = event_handler;
}

void quesync::client::event_handler::remove_event_handler(quesync::event_type type)
{
    // Remove the event handler for the event
    _event_handlers.erase(type);
}