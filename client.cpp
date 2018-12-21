#include <iostream>
#include "bass.h"

int main()
{
    // Check if the correct version of the BASS library has been loaded
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        std::cout << "Incorrect version of BASS library has been loaded! Exiting.." << std::endl;
    }

    // Print BASS version
    std::cout << "BASS library version: " << BASS_GetVersion() << std::endl;

    return 0;
}