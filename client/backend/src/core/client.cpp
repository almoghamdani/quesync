#include "client.h"

#include <iostream>

#include "./../../../shared/events/server_disconnect_event.h"

quesync::client::client::client()
    : _communicator(nullptr),
      _auth(nullptr),
      _users(nullptr),
      _channels(nullptr),
      _messages(nullptr),
      _voice(nullptr),
      _files(nullptr) {
    socket_manager::init();
}

void quesync::client::client::init() {
    // Init modules
    _communicator = std::make_shared<modules::communicator>(shared_from_this());
    _auth = std::make_shared<modules::auth>(shared_from_this());
    _users = std::make_shared<modules::users>(shared_from_this());
    _messages = std::make_shared<modules::messages>(shared_from_this());
    _channels = std::make_shared<modules::channels>(shared_from_this());
    _voice = std::make_shared<modules::voice>(shared_from_this());
    _files = std::make_shared<modules::files>(shared_from_this());
}

void quesync::client::client::connect(std::string server_ip) {
    // Try to connect to the server
    _communicator->connect(server_ip);

    // Call connected event on modules
    _auth->connected(server_ip);
    _users->connected(server_ip);
    _messages->connected(server_ip);
    _channels->connected(server_ip);
    _voice->connected(server_ip);
    _files->connected(server_ip);
}

void quesync::client::client::clean_connection() {
    std::shared_ptr<events::server_disconnect_event> server_disconnect_event =
        std::make_shared<events::server_disconnect_event>();

    // If the user is authenticated, disconnect it
    if (_auth->get_user()) {
        disconnected();
    }

    // Clean modules
    _auth->clean_connection();
    _users->clean_connection();
    _messages->clean_connection();
    _channels->clean_connection();
    _voice->clean_connection();
    _files->clean_connection();

    try {
        // Call the server disconnect event
        _communicator->event_handler().call_event(
            std::static_pointer_cast<event>(server_disconnect_event));
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

void quesync::client::client::disconnected() {
    // Send disconnected event to modules
    _auth->disconnected();
    _users->disconnected();
    _messages->disconnected();
    _channels->disconnected();
    _voice->disconnected();
    _files->disconnected();
}

std::shared_ptr<quesync::client::modules::communicator> quesync::client::client::communicator() {
    return _communicator;
}

std::shared_ptr<quesync::client::modules::auth> quesync::client::client::auth() { return _auth; }

std::shared_ptr<quesync::client::modules::users> quesync::client::client::users() { return _users; }

std::shared_ptr<quesync::client::modules::channels> quesync::client::client::channels() {
    return _channels;
}

std::shared_ptr<quesync::client::modules::messages> quesync::client::client::messages() {
    return _messages;
}

std::shared_ptr<quesync::client::modules::voice> quesync::client::client::voice() { return _voice; }

std::shared_ptr<quesync::client::modules::files> quesync::client::client::files() { return _files; }