#include <iostream>
#include "include/bass.h"
#include "include/opus.h"

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 2
#define BASS_RECORD_UPDATE_TIME 10

BOOL CALLBACK recordHandleProc(HRECORD handle, const void *buffer, DWORD length, void *user)
{
    // Get the encoder sent as an argument
    OpusEncoder *opusEncoder = (OpusEncoder *)user;
    unsigned char *encodedData = (unsigned char *)malloc(length);

    std::cout << opus_encode_float(opusEncoder, (const float *)buffer, 960, encodedData, length) << std::endl;

    return TRUE;
}

int main()
{
    int opusError;
    OpusEncoder *opusEncoder;
    HRECORD recordHandle;

    //* Check if the correct version of the BASS library has been loaded
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        std::cout << "Incorrect version of BASS library has been loaded! Exiting.." << std::endl;
        return 1;
    }

    // Print BASS version
    std::cout << "BASS library version: " << BASS_GetVersion() << std::endl;

    // Print Opus library version
    std::cout << "Opus library version: " << opus_get_version_string() << std::endl;

    

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

    // Create the opus encoder for the recording
    opusEncoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opusError);

    // Try and start recording
    recordHandle = BASS_RecordStart(RECORD_FREQUENCY, RECORD_CHANNELS, MAKELONG(BASS_SAMPLE_FLOAT, BASS_RECORD_UPDATE_TIME), recordHandleProc, opusEncoder);
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