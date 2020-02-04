#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../../shared/event.h"
#include "../../../../shared/event_type.h"

namespace quesync {
namespace client {
class event_handler {
   public:
    /**
     * Call event handler for an event.
     *
     * @param evt A shared pointer to the event object.
     */
    void call_event(std::shared_ptr<quesync::event> evt);

    /**
     * Register event handler for an event.
     *
     * @param type The type of the event.
     * @param event_handler The function to be called when the event is called.
     */
    void register_event_handler(quesync::event_type type,
                                std::function<void(std::shared_ptr<quesync::event>)> event_handler);

    /**
     * Register core event handler for an event.
     *
     * @param type The type of the event.
     * @param event_handler The function to be called when the event is called.
     */
    void register_core_event_handler(
        quesync::event_type type,
        std::function<void(std::shared_ptr<quesync::event>)> event_handler);

    /**
     * Clear all event handlers for event.
     *
     * @param type The type of the event.
     */
    void clear_event_handlers(quesync::event_type type);

    /**
     * Clear all event handlers.
     */
    void clear_all_event_handlers();

   private:
    std::unordered_map<quesync::event_type,
                       std::vector<std::function<void(std::shared_ptr<quesync::event>)>>>
        _event_handlers;

    std::unordered_map<quesync::event_type, std::function<void(std::shared_ptr<quesync::event>)>>
        _core_event_handlers;
};
};  // namespace client
};  // namespace quesync
