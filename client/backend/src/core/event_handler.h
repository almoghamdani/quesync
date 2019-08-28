#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "../../../../shared/event.h"
#include "../../../../shared/event_type.h"

namespace quesync {
namespace client {
class event_handler {
   public:
    void call_event(std::shared_ptr<quesync::event> evt);
    void register_event_handler(quesync::event_type type,
                                std::function<void(std::shared_ptr<quesync::event>)> event_handler);
    void remove_event_handler(quesync::event_type type);

   private:
    std::unordered_map<quesync::event_type, std::function<void(std::shared_ptr<quesync::event>)>> _event_handlers;
};
};  // namespace client
};  // namespace quesync
