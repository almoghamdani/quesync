#pragma once

#include <memory>

// Prevent loop header include
class Quesync;

class Manager
{
public:
    Manager(std::shared_ptr<Quesync> server) : _server(server) {}

protected:
    std::shared_ptr<Quesync> _server;
};