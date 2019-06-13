#include "event_manager.h"

#include "quesync.h"

#include "../shared/packets/event_packet.h"

EventManager::EventManager(std::shared_ptr<Quesync> server) : Manager(server)
{
}

void EventManager::triggerEvent(Event &evt, std::string target_user_id)
{
    std::shared_ptr<Session> target_session;
    EventPacket event_packet(evt);

    // Try to get the target authenticated session of the target user
    target_session = _server->userManager()->getAuthenticatedSessionOfUser(target_user_id);
    if (!target_session) // If not found return
    {
        return;
    }

    // Send to the target user the event packet serialized
    target_session->sendOnly(event_packet.encode());
}