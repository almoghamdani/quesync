#include <iostream>
#include "include/bass.h"

BOOL CALLBACK recordHandleProc(HRECORD handle, const void *buffer, DWORD length, void *user)
{
    std::cout << (*(char *)buffer) << std::endl;
    return TRUE;
}

int main()
{
    HRECORD recordHandle;

    //* Check if the correct version of the BASS library has been loaded
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        std::cout << "Incorrect version of BASS library has been loaded! Exiting.." << std::endl;
        return 1;
    }

    // Print BASS version
    std::cout << "BASS library version: " << BASS_GetVersion() << std::endl;

    // TODO: Add record device selection
    // Try to init the default record device
    if (!BASS_RecordInit(0))
    {
        // Print the BASS error
        switch(BASS_ErrorGetCode())
        {
        case BASS_ERROR_DX:
            std::cout << "Missing DirectX! Exiting.." << std::endl;
            break;

        case BASS_ERROR_DEVICE:
            std::cout << "Invalid record device selected! Exiting.." << std::endl;
            break;

        case BASS_ERROR_DRIVER:
            std::cout << "Missing device driver! Exiting.." << std::endl;
            break;
        }

        return 1;
    }

    // Try and start recording
    recordHandle = BASS_RecordStart(44100, 2, MAKELONG(0, 5), recordHandleProc, NULL);
    if (!recordHandle)
    {
        std::cout << "IDK" << std::endl;

        return 1;
    }

    // Infinity loop for this main thread
    while (true)
    {

    }

    return 0;
}