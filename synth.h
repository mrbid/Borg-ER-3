/*
    James William Fletcher (james@voxdsp.com)
        September 2021
    
    Borg ER-3
*/
#ifndef SYNTH_H
#define SYNTH_H

#include <SDL2/SDL.h>
#include <math.h>

// generators
float getSlantSine(float phase, float resolution);
float getImpulse(float phase, float resolution);
float getSquare(float phase, float resolution);
float getSawtooth(float phase, float resolution);
float getTriangle(float phase, float resolution);
float aliased_sin(float theta);

// utility functions
float Hz(float hz);
float squish(float f);
int fZero(float f);

// init
int initMonoAudio(int samplerate);

// file
void writeWAV(const char* file);

// play
void setSampleLen(Uint32 seconds);
void playSample();
void stopSample();


/*
    functions bodies
*/

// float getImpulse(float phase, float resolution)
// {
//     float yr = 0.f;

//     const float fa = 1.f;   // frequency start
//     const float ma = 0.5f;  // amplitude start
//     float mad = 1.f;        // amplitude divisor start
//     const float mam = 1.4f; // amplitude change over time

//     yr += sin(phase*(fa*1.f))  *  (ma/mad); mad *= mam;
//     yr += sin(phase*(fa*2.f))  * -(ma/mad); mad *= mam;
//     yr += sin(phase*(fa*3.f))  * -(ma/mad); mad *= mam;
//     yr += sin(phase*(fa*4.f))  *  (ma/mad); mad *= mam;
//     yr += sin(phase*(fa*5.f))  *  (ma/mad); mad *= mam;
//     yr += sin(phase*(fa*6.f))  * -(ma/mad); mad *= mam;
//     yr += sin(phase*(fa*7.f))  * -(ma/mad); mad *= mam;
//     yr += sin(phase*(fa*8.f))  *  (ma/mad); mad *= mam;
//     yr += sin(phase*(fa*9.f))  *  (ma/mad); mad *= mam;
//     yr += sin(phase*(fa*10.f)) * -(ma/mad); mad *= mam;
//     yr += sin(phase*(fa*11.f)) * -(ma/mad);
//     return yr;
// }

// float getSquare2(float phase, float resolution)
// {
//     resolution *= 2.f;
//     float yr = sin(phase);
//     for(float h = 1.f; h < resolution; h+=2.f)
//         if(h != 1.f)
//             yr += sin(phase*h)/h;
//     return yr;
// }

float aliased_sin(float theta)
{
    // data for the wavetable
    static float sine_wtable[65536] = {0};

    // called once on first execution
    if(sine_wtable[16384] < 0.5f)
    {
        for(int i = 0; i < 65536; i++)
            sine_wtable[i] = sin(i * 9.587380191e-05f); // 9.587380191e-05f = x2PIf / 65536.f;
        //printf("table generated: %f\n", sine_wtable[16384]);
    }

    // return result
    const unsigned short i = (unsigned short)(10430.37793f * theta); // 10430.37793f = 65536.f / x2PIf
    return sine_wtable[i];
}

float getSlantSine(float phase, float resolution)
{
    float yr = aliased_sin(phase);
    for(float h = 3.f; h < resolution; h+=1.f)
    {
        yr += (aliased_sin(phase*h) / (h*h));
    }
    return yr;
}

float getImpulse(float phase, float resolution)
{
    if(resolution == 0)
        resolution = 1;
    float yr = 0.f;

    const float fa = 1.f;   // frequency start
    const float ma = 0.5f;  // amplitude start
    float mad = 1.f;        // amplitude divisor start
    const float mam = 1.4f; // amplitude change over time

    for(float f = 0; f <= resolution; f++, mad *= mam)
        yr += aliased_sin(phase*(fa*f)) * (ma/mad);
    
    return yr;
}

float getSquare(float phase, float resolution)
{
    resolution *= 2.f;
    float yr = aliased_sin(phase);
    for(float h = 3.f; h < resolution; h+=2.f)
        yr += aliased_sin(phase*h)/h;
    return yr;
}

float getSawtooth(float phase, float resolution)
{
    float yr = aliased_sin(phase);
    for(float h = 2.f; h <= resolution; h+=1.f)
        yr += aliased_sin(phase*h)/h;
    return yr;
}

float getTriangle(float phase, float resolution)
{
    resolution *= 2.f;
    float yr = aliased_sin(phase);
    float sign = -1.f;
    for(float h = 3.f; h <= resolution; h+=2.f)
    {
        yr += (aliased_sin(phase*h) / (h*h)) * sign;
        sign *= -1.f;
    }
    return yr;
}

float Hz(float hz)
{
    return hz * 6.283185482f;
}

int fZero(float f)
{
    return f > -0.01 && f < 0.01 ? 1 : 0;
}

float squish(float f)
{
    return fabs(tanh(f));
}

// vars
#define MAX_SAMPLE 1455300 //33*44100
SDL_AudioSpec sdlaudioformat;
Sint8 sample[MAX_SAMPLE];
Uint32 sample_index = 0;
Uint32 sample_len = 0;

void audioCallback(void* unused, Uint8* stream, int len)
{
    if(sample_index > sample_len)
        return;
    
    for(int i = 0; i < len; i++)
    {
        stream[i] = sample[sample_index];
        sample_index++;
        if(sample_index > sample_len)
        {
            SDL_PauseAudio(1);
            return;
        }
    }
}

void playSample()
{
    sample_index = 0;
    SDL_PauseAudio(0);
}

void stopSample()
{
    sample_index = 0;
    SDL_PauseAudio(1);
}

void setSampleLen(Uint32 seconds)
{
    sample_len = sdlaudioformat.freq * seconds;
    if(sample_len >= MAX_SAMPLE)
        sample_len = MAX_SAMPLE-1;
}

int initMonoAudio(int samplerate)
{
    // set audio format
    sdlaudioformat.freq = samplerate; // 44100 / 48000
    sdlaudioformat.format = AUDIO_S8; // AUDIO_S16
    sdlaudioformat.channels = 1;
    sdlaudioformat.samples = 4096;
    sdlaudioformat.callback = audioCallback;
    sdlaudioformat.userdata = NULL;

    // open audio device
    if(SDL_OpenAudio(&sdlaudioformat, 0) < 0)
        return -1;

    // success
    return 1;
}

void writeWAV(const char* file)
{
    // I have to convert the buffer to unsigned, madness.
    Uint8 usample[MAX_SAMPLE];
    for(int i = 0; i < sample_len; i++)
        usample[i] = sample[i]+128;

    // prep header
    const unsigned int wavedata_size = sample_len + 44;
    const unsigned int subchunk = 16;
    const unsigned short audioformat = 1;
    const unsigned short channels = 1;
    const unsigned int samplerate = sdlaudioformat.freq;
    const unsigned short bitspersample = 8;
    const unsigned int byterate = (samplerate * channels * bitspersample) / 8;
    const unsigned short blockalignment = (channels * bitspersample) / 8;

    // write wav
    FILE* f = fopen(file, "wb");
    if(f != NULL)
    {
        fwrite("RIFF", 4, 1, f);
        fwrite(&wavedata_size, 4, 1, f);
        fwrite("WAVE", 4, 1, f);
        fwrite("fmt ", 4, 1, f);
        fwrite(&subchunk, 4, 1, f);
        fwrite(&audioformat, 2, 1, f);
        fwrite(&channels, 2, 1, f);
        fwrite(&samplerate, 4, 1, f);
        fwrite(&byterate, 4, 1, f);
        fwrite(&blockalignment, 2, 1, f);
        fwrite(&bitspersample, 2, 1, f);
        fwrite("data", 4, 1, f);
        fwrite(&sample_len, 4, 1, f);
        fwrite(usample, sample_len, 1, f);
        fclose(f);
    }
}

#endif
