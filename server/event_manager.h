#pragma once
#include "manager.h"

#include "../shared/event.h"

class EventManager : Manager
{
public:
    EventManager(std::shared_ptr<Quesync> server);

    void triggerEvent(Event& evt, std::string target_user_id);
};