#include "event_handler.h"

#include <magic_enum.hpp>
#include <stdexcept>

void quesync::client::event_handler::call_event(std::shared_ptr<quesync::event> evt) {
    try {
        // Call the core event handler
        _core_event_handlers[evt->type](evt);
    } catch (...) {
    }

    // Check for no event handlers
    if (!_event_handlers.count(evt->type) || !_event_handlers[evt->type].size()) {
        throw std::runtime_error("No event handlers found for '" +
                                 static_cast<std::string>(magic_enum::enum_name(evt->type)) + "'");
    }

    // For each event handler
    for (auto& func : _event_handlers[evt->type]) {
        try {
            // Call the event handler functions for the event with the event data
            func(evt);
        } catch (...) {
        }
    }
}

void quesync::client::event_handler::register_event_handler(
    event_type type, std::function<void(std::shared_ptr<quesync::event>)> event_handler) {
    // Init event handlers vector if not exists
    if (!_event_handlers.count(type)) {
        _event_handlers[type] = std::vector<std::function<void(std::shared_ptr<quesync::event>)>>();
    }

    // Add the event handler for the event
    _event_handlers[type].push_back(event_handler);
}

void quesync::client::event_handler::register_core_event_handler(
    event_type type, std::function<void(std::shared_ptr<quesync::event>)> event_handler) {
    // Set the core event handler for the event
    _core_event_handlers[type] = event_handler;
}

void quesync::client::event_handler::clear_event_handlers(quesync::event_type type) {
    // Remove the event handlers for the event
    _event_handlers.erase(type);
}

void quesync::client::event_handler::clear_all_event_handlers() {
    // Clear the event handlers
    _event_handlers.clear();
}