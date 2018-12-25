#pragma once
#include <SDKDDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WINXP // Define min sockets version of Windows XP

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

class SocketManager
{
public:
    static void initWinsock();
}
