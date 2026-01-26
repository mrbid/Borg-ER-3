/*
    James William Fletcher (github.com/mrbid)
        September 2021
    
    Borg ER-3

    Since the Borg is locked to 30 samples of
    oscillator harmonic resolution I have
    pre-computed reciprocal tables to this
    resolution for the divisions, this is a
    marginal optimisation mostly only
    noticable in benchmarking.
    (i say 30 but its 29 due to the < op)
*/
#ifndef SYNTH_H
#define SYNTH_H

#include <SDL2/SDL.h>
#include <math.h>

#define USE_RECIPROCAL_TABLES

// generators
float getSlantSine(float phase, float resolution);
float getSquare(float phase, float resolution);
float getSawtooth(float phase, float resolution);
float getTriangle(float phase, float resolution);
float getImpulse(float phase, float resolution);
float getViolin(float phase, float resolution);
float aliased_sin(float theta);

// utility functions
float Hz(float hz);
float squish(float f);
int fZero(float f);
Sint8 quantise_float(float f);

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

#ifdef __x86_64__ 
float sine_wtable[65536] = {0};
#endif
inline float aliased_sin(float theta)
{
#ifdef __x86_64__ 
    const unsigned short i = (unsigned short)(10430.37793f * theta); // 10430.37793f = 65536.f / x2PIf
    return sine_wtable[i];
#else
    return sinf(theta);
#endif
}

inline float aliased_cos(float theta)
{
    return aliased_sin(1.570796371f - theta);
}

// reciprocal tables
#ifdef USE_RECIPROCAL_TABLES
    const float ht[] = {0.5f, 0.333333f, 0.25f, 0.2f, 0.166667f, 0.142857f, 0.125f, 0.111111f, 0.1f, 0.0909091f, 0.0833333f, 0.0769231f, 0.0714286f, 0.0666667f, 0.0625f, 0.0588235f, 0.0555556f, 0.0526316f, 0.05f, 0.047619f, 0.0454545f, 0.0434783f, 0.0416667f, 0.04f, 0.0384615f, 0.037037f, 0.0357143f, 0.0344828f, 0.0333333f, 0.0322581f, 0.03125f, 0.030303f, 0.0294118f, 0.0285714f, 0.0277778f, 0.027027f, 0.0263158f, 0.025641f, 0.025f, 0.0243902f, 0.0238095f, 0.0232558f, 0.0227273f, 0.0222222f, 0.0217391f, 0.0212766f, 0.0208333f, 0.0204082f, 0.02f, 0.0196078f, 0.0192308f, 0.0188679f, 0.0185185f, 0.0181818f, 0.0178571f, 0.0175439f, 0.0172414f, 0.0169492f};
    const float hht[] = {0.111111f, 0.0625f, 0.04f, 0.0277778f, 0.0204082f, 0.015625f, 0.0123457f, 0.01f, 0.00826446f, 0.00694444f, 0.00591716f, 0.00510204f, 0.00444444f, 0.00390625f, 0.00346021f, 0.00308642f, 0.00277008f, 0.0025f, 0.00226757f, 0.00206612f, 0.00189036f, 0.00173611f, 0.0016f, 0.00147929f, 0.00137174f, 0.00127551f, 0.00118906f, 0.00111111f, 0.00104058f, 0.000976562f, 0.000918274f, 0.000865052f, 0.000816327f, 0.000771605f, 0.00073046f, 0.000692521f, 0.000657462f, 0.000625f, 0.000594884f, 0.000566893f, 0.000540833f, 0.000516529f, 0.000493827f, 0.00047259f, 0.000452694f, 0.000434028f, 0.000416493f, 0.0004f, 0.000384468f, 0.000369822f, 0.000355999f, 0.000342936f, 0.000330579f, 0.000318878f, 0.000307787f, 0.000297265f, 0.000287274f};
    const float hhht[] = {1.f, 0.5f, 0.333333f, 0.25f, 0.2f, 0.166667f, 0.142857f, 0.125f, 0.111111f, 0.1f, 0.0909091f, 0.0833333f, 0.0769231f, 0.0714286f, 0.0666667f, 0.0625f, 0.0588235f, 0.0555556f, 0.0526316f, 0.05f, 0.047619f, 0.0454545f, 0.0434783f, 0.0416667f, 0.04f, 0.0384615f, 0.037037f, 0.0357143f, 0.0344828f, 0.0333333f};
#endif

#ifdef USE_RECIPROCAL_TABLES
    float getSlantSine(float phase, float resolution)
    {
        float yr = aliased_sin(phase);
        int i = 0;
        for(float h = 3.f; h < resolution; h+=1.f)
        {
            yr += aliased_sin(phase*h) * hht[i];
            i++;
        }
        return yr;
    }

    float getSquare(float phase, float resolution)
    {
        resolution *= 2.f;
        float yr = aliased_sin(phase);
        int i = 1;
        for(float h = 3.f; h < resolution; h+=2.f)
        {
            yr += aliased_sin(phase*h)*ht[i];
            i+=2;
        }
        return yr;
    }

    float getSawtooth(float phase, float resolution)
    {
        float yr = aliased_sin(phase);
        int i = 0;
        for(float h = 2.f; h <= resolution; h+=1.f)
        {
            yr += aliased_sin(phase*h)*ht[i];
            i++;
        }
        return yr;
    }

    float getTriangle(float phase, float resolution)
    {
        resolution *= 2.f;
        float yr = aliased_sin(phase);
        float sign = -1.f;
        int i = 0;
        for(float h = 3.f; h <= resolution; h+=2.f)
        {
            yr += (aliased_sin(phase*h) * hht[i]) * sign;
            sign *= -1.f;
            i+=2;
        }
        return yr;
    }

    float getBipulse(float phase, float resolution) // formant
    {
        float yr = 0.f;
        int i = 1;
        for(float h = 1.f; h <= resolution; h+=1.f)
        {
            const float d = (h - 5.f) * 0.5f;
            const float amp = expf(-d * d) * hhht[i];
            yr += aliased_sin(phase * h) * amp;
            i++;
        }

        return yr;
    }
#else
    float getSlantSine(float phase, float resolution)
    {
        float yr = aliased_sin(phase);
        for(float h = 3.f; h < resolution; h+=1.f)
        {
            yr += aliased_sin(phase*h) / (h*h);
        }
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

    float getBipulse(float phase, float resolution) // formant
    {
        float yr = 0.f;
        for(float h = 1.f; h <= resolution; h+=1.f)
        {
            const float d = (h - 5.f) * 0.5f;
            const float amp = expf(-d * d) / h;
            yr += aliased_sin(phase * h) * amp;
        }

        return yr;
    }
#endif

const float vamps[10] = {0.5f,0.45f,0.4f,0.35f,0.3f,0.25f,0.2f,0.15f,0.1f,0.05f};
float getViolin(float phase, float resolution) // Band-limited Impulse
{
    float yr = 0.f;
    for(int h = 0; h < 10; ++h)
    {
        float step = h * 3.f;
        if(resolution > step)
        {
            float phaseOffset = (h % 2 == 0) ? 0.f : 1.57079632679f;
            float amp = vamps[h];
            if(resolution < step + 3.f)
            {
                float t = (resolution - step) * 0.33333333333333333333f;
                amp *= t;
            }
            yr += aliased_sin(phase * (h + 1) + phaseOffset) * amp;
        }
    }

    return yr;
}

// --------------------------------------------- >

inline float Hz(float hz)
{
    return hz * 6.283185482f;
}

inline int fZero(float f)
{
    return f > -0.01f && f < 0.01f ? 1.f : 0.f;
}

inline float squish(float f)
{
    return fabsf(tanhf(f));
}

inline Sint8 quantise_float(float f)
{
    return roundf(f);
    // if(f < 0.f)
    //     f -= 0.5f;
    // else
    //     f += 0.5f;
    // return (Sint8)f;
}

// vars
#define MAX_SAMPLE 1455300 //33*44100
SDL_AudioSpec sdlaudioformat;
Sint8 sample[MAX_SAMPLE];
Uint32 sample_index = 0;
Uint32 sample_len = 0;

void audioCallback(void* unused, Uint8* stream, int len)
{    
    for(int i = 0; i < len; i++)
    {
        if(sample_index >= sample_len)
        {
            stream[i] = 0;
            SDL_PauseAudio(1);
            continue;
        }

        stream[i] = sample[sample_index];
        sample_index++;
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
    if(sample_len > MAX_SAMPLE)
        sample_len = MAX_SAMPLE;
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

#ifdef __x86_64__ 
    // generate sine table
    for(int i = 0; i < 65536; i++)
        sine_wtable[i] = sinf(i * 9.587380191e-05f); // 9.587380191e-05f = x2PIf / 65536.f;
#endif

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
