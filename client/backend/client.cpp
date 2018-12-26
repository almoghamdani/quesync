#include <iostream>

#include "include/socket-manager.hpp"
#include "include/voice-chat.hpp"

int main()
{
    VoiceChat *voiceChatManager;

    // Try to initate the winsock lib
    try {
        SocketManager::initWinsock();
    } catch (SocketError &ex) {
        std::cout << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Try to construct the voice chat manager
    try {
        voiceChatManager = new VoiceChat("127.0.0.1");
    } catch (SocketError &ex) {
        std::cout << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true)
    {

    }

    return 0;
}