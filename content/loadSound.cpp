
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

void LoadWav(EngineState* engineState, Sound* sound)
{
    engineState->platformReadFile(engineState->scratchBuffer, sound->filename);
    sound->loaded = false;

    WAV_HEADER* header = (WAV_HEADER*)engineState->scratchBuffer;
    int16* data = (int16*)(((uint8*)engineState->scratchBuffer) + sizeof(WAV_HEADER));
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

    sound->data = ArenaPushArray(&engineState->arenaHotreload, destinationSampleCount, Sample, sound->filename);

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

Sound* FileReadSound(EngineState* memory, const char* filename)
{
    Sound* sound = ArrayAddNew(memory->sounds);
    StringCopy(sound->filename, filename);
    LoadWav(memory, sound);
    return sound;
}
