#pragma once

#include<stdio.h> // For Printf
#include<stdlib.h> // For exit
#include<memory.h> // For memset
#include<math.h>

// OpenAL header files
#include<AL/al.h>
#include<AL/alc.h>

// Global Variables for OpenAL
ALCdevice* device;
ALCcontext* alContext;
ALuint buffer, source;


int audioInitialization()
{
    // OpenAL initialization

	// Initialize OpenAL
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        return -3;
    }

    alContext = alcCreateContext(device, NULL);
    if (!alContext) {
        printf("Failed to create OpenAL context\n");
        return -4;
    }

    if (!alcMakeContextCurrent(alContext)) {
        printf("Failed to make OpenAL context current\n");
        return -5;
    }

    // Generate OpemAL buffer and source
    alGenSources(1, &source);
    alGenBuffers(1, &buffer);

  // Load audio file
    ALuint frequency, format, size;
    ALvoid* data;
    ALsizei dataSize;
    ALboolean loop = AL_FALSE;
    ALenum error;

// Specify your audio file path here
    const char* audioFilePath = "01_WatanKiRaahMein.wav";

    FILE* audioFile = fopen(audioFilePath, "rb");
    if (!audioFile) {
        printf("Failed to open audio file\n");
        return -6;
    }

    // Get audio file data
    fseek(audioFile, 0, SEEK_END);
    size = ftell(audioFile);
    rewind(audioFile);
    data = malloc(size);
    if (!data) {
        printf("Failed to allocate memory for audio data\n");
        return -7;
    }

    fread(data, 1, size, audioFile);
    fclose(audioFile);

    // Determine audio format
    format = AL_FORMAT_MONO16; 
    frequency = 44100; 

    // Upload audio data to buffer
    alBufferData(buffer, format, data, size, frequency);
    free(data);

    // Attach buffer to source
    alSourcei(source, AL_BUFFER, buffer);

    // Set source parameters
    alSourcef(source, AL_PITCH, 2.0f);
    alSourcef(source, AL_GAIN, 2.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, loop);

    // Check for errors
    error = alGetError();
    if (error != AL_NO_ERROR) {
        printf("OpenAL error: %s\n", alGetString(error));
        return -7;
    }

	  // Start playing the audio
    alSourcePlay(source);

    return 0;

}