#include <iostream>
#include "bass.h"

int main()
{
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

    return 0;
}