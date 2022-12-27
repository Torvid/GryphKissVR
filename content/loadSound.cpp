#pragma once

#include "haven.cpp"

struct WAV_HEADER
{
    // RIFF Header
    uint8 riff_header[4]; // Contains "RIFF"
    int32 wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    uint8 wave_header[4]; // Contains "WAVE"

    // Format Header
    uint8 fmt_header[4]; // Contains "fmt " (includes trailing space)
    int32 fmt_chunk_size; // Should be 16 for PCM
    int16 audio_format; // Should be 1 for PCM. 3 for IEEE Float
    int16 num_channels;
    int32 sample_rate;
    int32 byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    int16 sample_alignment; // num_channels * Bytes Per Sample
    int16 bit_depth; // Number of bits per sample

    // Data
    uint8 data_header[4]; // Contains "data"
    int32 data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    //uint8 bytes; // Remainder of wave file is bytes
};

void LoadWav(Sound* sound)
{
    haven->platformReadFile(haven->scratchBuffer, sound->filename);
    sound->loaded = false;

    WAV_HEADER* header = (WAV_HEADER*)haven->scratchBuffer;
    int16* data = (int16*)(((uint8*)haven->scratchBuffer) + sizeof(WAV_HEADER));
    int32 sampleCount = header->data_bytes / 2; // divide by 2 because it's 16 bits per sample.

    // if the format is anything other than PCM, don't load it.
    if (header->audio_format != 1)
        return;

    // The game wants all sound in 44.1kHz, int16 [left, right, left, right, left, right]
    // Here we resample and realign to make it so.

    sampleCount = sampleCount / header->num_channels;
    float lengthInSeconds = (float)(sampleCount) / (float)header->sample_rate;
    int destinationSampleCount = lengthInSeconds * 44100;
    sound->length = lengthInSeconds;

    sound->data = ArenaPushArray(&haven->arenaHotreload, destinationSampleCount, Sample, sound->filename);

    Clear((uint8*)sound->data, destinationSampleCount * sizeof(Sample));

    sound->sampleCount = destinationSampleCount;

    int16* src = (int16*)data;
    Sample* dest = sound->data;
    float t;
    int s;
    // Resample
    for (int i = 0; i < destinationSampleCount; i++)
    {
        t = (float)i / (float)(destinationSampleCount - 1);
        s = lerp(0.0f, sampleCount - 1, t);

        if (header->num_channels == 1) // mono
        {
            dest[i].left = src[s];
            dest[i].right = src[s];
        }
        else if (header->num_channels == 2) // stereo
        {
            dest[i].left = src[s * 2];
            dest[i].right = src[s * 2 + 1];
        }
    }
    sound->loaded = true;
}

Sound* FileReadSound(const char* filename)
{
    ProfilerBeingSample();
    Sound* sound = ArrayAddNew(haven->sounds);
    StringCopy(sound->filename, filename);
    LoadWav(sound);
    ProfilerEndSample("LoadWav");
    return sound;
}


enum SoundFalloffType
{
    SoundFalloffType_Linear,
    SoundFalloffType_Exponential,
    SoundFalloffType_None,
};

struct SoundChannel
{
    bool playing;
    int currentSample;
    Sound* sound;
    bool looping;
    float volume;
    bool is3D;
    float3 position;
};

void PlaySound(Sound* sound, float volume = 1.0f, bool looping = false)
{
    SoundChannel* foundChannel = 0;
    for (int i = 0; i < ArrayCapacity(haven->soundChannels); i++)
    {
        if (!haven->soundChannels[i]->playing)
        {
            foundChannel = haven->soundChannels[i];
            break;
        }
    }

    if (!foundChannel)
        return;

    StructClear(foundChannel);

    foundChannel->playing = true;
    foundChannel->sound = sound;
    foundChannel->volume = volume;
    foundChannel->looping = looping;

}

void soundStart(GameMemory* memory)
{

}

void soundUpdate(GameMemory* memory)
{
    for (int i = 0; i < memory->SampleCount; i++)
    {
        memory->Samples[i].left = 0;
        memory->Samples[i].right = 0;
    }

    // Sound
    for (int j = 0; j < ArrayCapacity(haven->soundChannels); j++)
    {
        for (int i = 0; i < memory->SampleCount; i++)
        {
            SoundChannel* channel = haven->soundChannels[j];

            if (!channel->playing)
                break;

            Sound* sound = channel->sound;

            if (!sound)
                break;

            float x = ((input->mousePos.x / 2500.0f) - 0.5) * 2 * 16;
            int s1 = clamp(channel->currentSample - x, 0, (sound->sampleCount - 1));
            int s2 = clamp(channel->currentSample + x, 0, (sound->sampleCount - 1));
            Sample sample1 = sound->data[s1];
            Sample sample2 = sound->data[s2];

            int left = memory->Samples[i].left + (sample1.left * channel->volume * 0.5f);
            int right = memory->Samples[i].right + (sample2.right * channel->volume * 0.5f);

            if (!memory->gameFocused)
            {
                left = 0;
                right = 0;
            }

            memory->Samples[i].left = clamp(left, -32768, 32767);
            memory->Samples[i].right = clamp(right, -32768, 32767);

            channel->currentSample++;
            if (channel->currentSample > (sound->sampleCount - 1)) // went over, loop or stop.
            {
                if (channel->looping)
                    channel->currentSample = 0;
                else
                    channel->playing = false;
            }
        }
    }

}
