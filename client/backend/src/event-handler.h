#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

class EventHandler
{
  public:
    void callEvent(std::string event_name, nlohmann::json event_data);
    void registerEventHandler(std::string& event_name, std::function<void(nlohmann::json&)> event_handler);

  private:
    std::unordered_map<std::string, std::function<void(nlohmann::json&)>> _event_handlers;
};