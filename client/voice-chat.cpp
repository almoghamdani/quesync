#include "include/voice-chat.hpp"

using std::cout, std::endl;

VoiceChat::VoiceChat(const char *serverIP)
{
    std::thread recvThread, sendThread;
    const char *portStr = std::to_string(VOICE_CHAT_PORT).c_str();

    // * Create the UDP socket for communication with the voice chat server, a non-blocking port
    cout << "Initalizing voice chat socket" << endl;
    _voiceSocket = SocketManager::createUDPSocket(serverIP, portStr, true);
    
    // Create the receive voice chat thread and detach it
    recvThread = std::thread(&VoiceChat::receiveVoiceThread);
    recvThread.detach();

    // Create the send voice chat thread and detach it
    sendThread = std::thread(&VoiceChat::sendVoiceThread);
    sendThread.detach();
}