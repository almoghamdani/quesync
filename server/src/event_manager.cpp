#include "event_manager.h"

#include "server.h"

#include "../../shared/packets/event_packet.h"

quesync::server::event_manager::event_manager(std::shared_ptr<quesync::server::server> server)
    : manager(server) {}

void quesync::server::event_manager::trigger_event(std::shared_ptr<event> evt,
                                                   std::string target_user_id) {
    std::shared_ptr<session> target_session;
    packets::event_packet event_packet(evt);

    // Try to get the target authenticated session of the target user
    target_session = _server->user_manager()->get_authenticated_session_of_user(target_user_id);
    if (!target_session)  // If not found return
    {
        return;
    }

    // Send to the target user the event packet serialized
    target_session->send_only(event_packet.encode());
}