#pragma once

#include "user_management.h"
#include "../shared/event.h"

class EventManagement
{
public:
    static void TriggerEvent(UserManagement& user_management, Event& evt, std::string target_user_id);
};