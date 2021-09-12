/*
    James William Fletcher (james@voxdsp.com)
        September 2021
    
            Borg ER-3

    Everything is SDL apart from file I/O which is stdio.

    Could have used:
    https://wiki.libsdl.org/CategoryIO

    But I like stdio and I saw no benefit to SDL IO.

    The Borg ER-2 was in C++ and this time I decided to
    do it all in GNU C.
*/
#include <SDL2/SDL.h>
#include <stdio.h>

#include "sdl_extra.h"
#include "synth.h"
#include "res.h"

#define SAMPLE_RATE   44100

#define MAXFREQUENCY  1800 //20000
#define MAXAMPLITUDE  128 //SDL_MIX_MAXVOLUME
#define MAXRESOLUTION 30
#define MAXTRANSITION 1
#define MAXBIQUAD     6.3
#define MAXOFFSETF    1
#define MAXOFFSETA    MAXAMPLITUDE
#define MAXCRUSH      1
#define MAXSAMPLELEN  33

#define wlerp(a, b, i) ((b - a) * i + a)

SDL_Window *window = NULL;
char *basedir, *appdir;

Uint32 theme_type = 1;
Uint32 themeon = 0;

SDL_Surface* bb; // backbuffer
SDL_Surface* s_bg;
SDL_Surface* s_icon;
SDL_Surface* s_rm;
SDL_Surface* s_tm;
SDL_Surface* s_plus;
SDL_Surface* s_minus;
SDL_Surface* s_mul;

Uint32 bgcolor = 0xFFE8A200;
Uint32 outlinecolor = 0xFF000000;
Uint32 scopecolor = 0xFF363636;
Uint32 select_lightness = 44;

Uint8 selected_bank = 0;
Sint32 selected_dial = -1;
Uint8 envelope_enabled = 0;

SDL_Rect screen_rect = {0, 0, 480, 435};
SDL_Rect bankl_rect = {6, 416, 12, 14};
SDL_Rect bankr_rect = {66, 416, 12, 14};
SDL_Rect load_rect = {80, 416, 46, 14};
SDL_Rect save_rect = {128, 416, 46, 14};
SDL_Rect theme_rect = {218, 417, 44, 12};
SDL_Rect secl_rect = {306, 416, 12, 14};
SDL_Rect secr_rect = {366, 416, 12, 14};
SDL_Rect export_rect = {380, 416, 46, 14};
SDL_Rect play_rect = {428, 416, 46, 14};
SDL_Rect envelope_rect = {7, 155, 466, 126};

SDL_Rect dial_rect[50];
SDL_Rect mul_rect[10];
SDL_Rect fm_rect[10];
SDL_Rect am_rect[10];

float dial_scale[] = {MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXFREQUENCY, MAXAMPLITUDE, MAXRESOLUTION, MAXTRANSITION, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXBIQUAD, MAXOFFSETF, MAXOFFSETA, MAXCRUSH};
Uint8 dial_neg[] = {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0};

struct ssynth
{
    Uint8 seclen;
    Uint8 select_mode;

    float envelope[466]; // 0-1

    Uint8 am_state[10];
    Uint8 mul_state[10];
    Uint8 fm_state[10];
    
    float dial_state[50]; // 0-1
};
struct ssynth synth[256];

void saveState()
{
    char file[256];
    sprintf(file, "%sbank.save", appdir);
    FILE* f = fopen(file, "wb");
    if(f != NULL)
    {
        unsigned int strikeout = 0;
        while(fwrite(&synth[0], sizeof(struct ssynth), 256, f) != 256)
        {
            printf("Writing bank failed... Trying again.\n");
            strikeout++;
            if(strikeout > 3333)
            {
                printf("Saving your data totally failed. Outch. :(\n");
                break;
            }
        }
        fclose(f);
    }
}

void loadState()
{
    char file[256];
    sprintf(file, "%sbank.save", appdir);
    FILE* f = fopen(file, "rb");
    if(f != NULL)
    {
        unsigned int strikeout = 0;
        while(fread(&synth[0], sizeof(struct ssynth), 256, f) != 256)
        {
            printf("Read bank failed... Trying again.\n");
            strikeout++;
            if(strikeout > 3333)
            {
                printf("Loading your data totally failed. ¯\\_(ツ)_/¯ Maybe it's corrupted? :(\n");
                break;
            }
        }
        fclose(f);
    }
}

float oscphase[8] = {0.f}; // oscillator phases
float doOsc(Uint32 oscid, float input1, float input2)
{
    static float reciprocal_sample_rate = 0.f;
    float o;
    float f,a,r,t;
    Uint8 input1_fmmod = 0, input1_ammod = 0, input1_mod = 0;
    Uint8 input2_fmmod = 0, input2_ammod = 0, input2_mod = 0;

    if(reciprocal_sample_rate == 0)
        reciprocal_sample_rate = 1.f/(float)SAMPLE_RATE;

    // load selected oscillator dial values with scaling
    if(oscid == 1)
    {
        f = synth[selected_bank].dial_state[16] * dial_scale[16];
        a = synth[selected_bank].dial_state[17] * dial_scale[17];
        r = synth[selected_bank].dial_state[18] * dial_scale[18];
        t = synth[selected_bank].dial_state[19] * dial_scale[19];

        input1_ammod = synth[selected_bank].am_state[9];
        input1_mod = synth[selected_bank].mul_state[9];
        input1_fmmod = synth[selected_bank].fm_state[9];

        input2_ammod = synth[selected_bank].am_state[8];
        input2_mod = synth[selected_bank].mul_state[8];
        input2_fmmod = synth[selected_bank].fm_state[8];
    }
    else if(oscid == 2)
    {
        f = synth[selected_bank].dial_state[20] * dial_scale[20];
        a = synth[selected_bank].dial_state[21] * dial_scale[21];
        r = synth[selected_bank].dial_state[22] * dial_scale[22];
        t = synth[selected_bank].dial_state[23] * dial_scale[23];

        input1_ammod = synth[selected_bank].am_state[6];
        input1_mod = synth[selected_bank].mul_state[6];
        input1_fmmod = synth[selected_bank].fm_state[6];

        input2_ammod = synth[selected_bank].am_state[5];
        input2_mod = synth[selected_bank].mul_state[5];
        input2_fmmod = synth[selected_bank].fm_state[5];
    }
    else if(oscid == 3)
    {
        f = synth[selected_bank].dial_state[24] * dial_scale[24];
        a = synth[selected_bank].dial_state[25] * dial_scale[25];
        r = synth[selected_bank].dial_state[26] * dial_scale[26];
        t = synth[selected_bank].dial_state[27] * dial_scale[27];

        input1_ammod = synth[selected_bank].am_state[3];
        input1_mod = synth[selected_bank].mul_state[3];
        input1_fmmod = synth[selected_bank].fm_state[3];

        input2_ammod = synth[selected_bank].am_state[2];
        input2_mod = synth[selected_bank].mul_state[2];
        input2_fmmod = synth[selected_bank].fm_state[2];
    }
    else if(oscid == 4)
    {
        f = synth[selected_bank].dial_state[28] * dial_scale[28];
        a = synth[selected_bank].dial_state[29] * dial_scale[29];
        r = synth[selected_bank].dial_state[30] * dial_scale[30];
        t = synth[selected_bank].dial_state[31] * dial_scale[31];

        input1_ammod = synth[selected_bank].am_state[0];
        input1_mod = synth[selected_bank].mul_state[0];
        input1_fmmod = synth[selected_bank].fm_state[0];

        input2_ammod = 0;
        input2_mod = 0;
        input2_fmmod = 0;
    }
    else if(oscid == 5)
    {
        f = synth[selected_bank].dial_state[0] * dial_scale[0];
        a = synth[selected_bank].dial_state[1] * dial_scale[1];
        r = synth[selected_bank].dial_state[2] * dial_scale[2];
        t = synth[selected_bank].dial_state[3] * dial_scale[3];
        
        input1_ammod = synth[selected_bank].am_state[7];
        input1_mod = synth[selected_bank].mul_state[7];
        input1_fmmod = synth[selected_bank].fm_state[7];

        input2_ammod = 0;
        input2_mod = 0;
        input2_fmmod = 0;
    }
    else if(oscid == 6)
    {
        f = synth[selected_bank].dial_state[4] * dial_scale[4];
        a = synth[selected_bank].dial_state[5] * dial_scale[5];
        r = synth[selected_bank].dial_state[6] * dial_scale[6];
        t = synth[selected_bank].dial_state[7] * dial_scale[7];

        input1_ammod = synth[selected_bank].am_state[4];
        input1_mod = synth[selected_bank].mul_state[4];
        input1_fmmod = synth[selected_bank].fm_state[4];

        input2_ammod = 0;
        input2_mod = 0;
        input2_fmmod = 0;
    }
    else if(oscid == 7)
    {
        f = synth[selected_bank].dial_state[8] * dial_scale[8];
        a = synth[selected_bank].dial_state[9] * dial_scale[9];
        r = synth[selected_bank].dial_state[10] * dial_scale[10];
        t = synth[selected_bank].dial_state[11] * dial_scale[11];

        input1_ammod = synth[selected_bank].am_state[1];
        input1_mod = synth[selected_bank].mul_state[1];
        input1_fmmod = synth[selected_bank].fm_state[1];

        input2_ammod = 0;
        input2_mod = 0;
        input2_fmmod = 0;
    }
    else if(oscid == 8)
    {
        f = synth[selected_bank].dial_state[12] * dial_scale[12];
        a = synth[selected_bank].dial_state[13] * dial_scale[13];
        r = synth[selected_bank].dial_state[14] * dial_scale[14];
        t = synth[selected_bank].dial_state[15] * dial_scale[15];

        input1_ammod = 0;
        input1_mod = 0;
        input1_fmmod = 0;

        input2_ammod = 0;
        input2_mod = 0;
        input2_fmmod = 0;
    }

    // fm modulation inputs
    if(input1_fmmod != 0)
    {
        if(input1_fmmod == 1)
            f *= input1;
        else if(input1_fmmod == 2)
            t *= squish(input1);
        else if(input1_fmmod == 3)
            r *= squish(input1);
    }

    if(input2_fmmod != 0)
    {
        if(input2_fmmod == 1)
            f *= input2;
        else if(input2_fmmod == 2)
            t *= squish(input2);
        else if(input2_fmmod == 3)
            r *= squish(input2);
    }

    // am modulation inputs
    if(input1_ammod != 0)
    {
        if(input1_ammod == 1)
            a *= input1;
        else if(input1_ammod == 2)
            r *= squish(input1);
        else if(input1_ammod == 3)
            t *= squish(input1);
    }

    if(input2_ammod != 0)
    {
        if(input2_ammod == 1)
            a *= input2;
        else if(input2_ammod == 2)
            r *= squish(input2);
        else if(input2_ammod == 3)
            t *= squish(input2);
    }

    // should be blending between these shapess
    if(t <= 0.1666666716f)
        o = sin(oscphase[oscid]) * a;
    else if(t <= 0.3333333433f)
        o = getSlantSine(oscphase[oscid], r) * a;
    else if(t <= 0.50f)
        o = getSquare(oscphase[oscid], r) * a;
    else if(t <= 0.6666666865f)
        o = getSawtooth(oscphase[oscid], r) * a;
    else if(t <= 0.8333333731f)
        o = getTriangle(oscphase[oscid], r) * a;
    else
        o = getImpulse(oscphase[oscid], r) * a;

    // add/sub/mul modulation inputs
    if(input1_mod != 0)
    {
        if(input1_mod == 1)
            o += input1;
        else if(input1_mod == 2)
            o -= input1;
        else if(input1_mod == 3)
            o *= input1;
    }

    if(input2_mod != 0)
    {
        if(input2_mod == 1)
            o += input2;
        else if(input2_mod == 2)
            o -= input2;
        else if(input2_mod == 3)
            o *= input2;
    }

    // step oscillator phase
    oscphase[oscid] += Hz(f)*reciprocal_sample_rate;

    // return output
    return o;
}

Uint32 eic = 0;
Uint32 samstep = 0;
Uint32 envelope_offset = 0;
Uint32 crush_len = 0;
Uint32 crush_index = 0;
float  crush_value = 0.f;
float a_i1, a_i2, a_o1, a_o2;
float b_i1, b_i2, b_o1, b_o2;
float c_i1, c_i2, c_o1, c_o2;
float doFilters(float f)
{
    // pre-calculate vars
    static float r_samstep = 0.f;
    if(samstep == 0)
    {
        samstep = sample_len / 466;
        r_samstep = 1.f/(float)samstep;
    }

    // crush
    if(crush_len != 0)
    {   
        crush_index++;
        if(crush_index >= crush_len)
            crush_index = 0;
        else
            return crush_value;
    }

    // biquad dials
    const float a_b1 = synth[selected_bank].dial_state[32] * dial_scale[32];
    const float a_b2 = synth[selected_bank].dial_state[33] * dial_scale[33];
    const float a_b3 = synth[selected_bank].dial_state[34] * dial_scale[34];
    const float a_a1 = synth[selected_bank].dial_state[35] * dial_scale[35];
    const float a_a2 = synth[selected_bank].dial_state[36] * dial_scale[36];

    const float b_b1 = synth[selected_bank].dial_state[37] * dial_scale[37];
    const float b_b2 = synth[selected_bank].dial_state[38] * dial_scale[38];
    const float b_b3 = synth[selected_bank].dial_state[39] * dial_scale[39];
    const float b_a1 = synth[selected_bank].dial_state[40] * dial_scale[40];
    const float b_a2 = synth[selected_bank].dial_state[41] * dial_scale[41];

    const float c_b1 = synth[selected_bank].dial_state[42] * dial_scale[42];
    const float c_b2 = synth[selected_bank].dial_state[43] * dial_scale[43];
    const float c_b3 = synth[selected_bank].dial_state[44] * dial_scale[44];
    const float c_a1 = synth[selected_bank].dial_state[45] * dial_scale[45];
    const float c_a2 = synth[selected_bank].dial_state[46] * dial_scale[46];

    // biquad 1
    if(fZero(a_b1) != 1 || fZero(a_b2) != 1 || fZero(a_b3) != 1 || fZero(a_a1) != 1 || fZero(a_a2) != 1)
    {
        const float a_out =   a_b1 * f
                            + a_b2 * a_i1
                            + a_b3 * a_i2
                            - a_a1 * a_o1
                            - a_a2 * a_o2;
        a_i2 = a_i1;
        a_i1 = f;
        a_o2 = a_o1;
        a_o1 = a_out;

        f = a_out;
    }

    // biquad 2
    if(fZero(b_b1) != 1 || fZero(b_b2) != 1 || fZero(b_b3) != 1 || fZero(b_a1) != 1 || fZero(b_a2) != 1)
    {
        const float b_out =   b_b1 * f
                            + b_b2 * b_i1
                            + b_b3 * b_i2
                            - b_a1 * b_o1
                            - b_a2 * b_o2;
        b_i2 = b_i1;
        b_i1 = f;
        b_o2 = b_o1;
        b_o1 = b_out;

        f = b_out;
    }

    // biquad 3
    if(fZero(c_b1) != 1 || fZero(c_b2) != 1 || fZero(c_b3) != 1 || fZero(c_a1) != 1 || fZero(c_a2) != 1)
    {
        const float c_out =   c_b1 * f
                            + c_b2 * c_i1
                            + c_b3 * c_i2
                            - c_a1 * c_o1
                            - c_a2 * c_o2;
        c_i2 = c_i1;
        c_i1 = f;
        c_o2 = c_o1;
        c_o1 = c_out;

        f = c_out;
    }
    
    // scale by lerped envelope
    f *= wlerp(synth[selected_bank].envelope[envelope_offset], synth[selected_bank].envelope[envelope_offset+1], ((float)eic)*r_samstep);

    // apply offsets
    f -= synth[selected_bank].dial_state[48] * dial_scale[48];

    // crush
    if(crush_len != 0)
        crush_value = f;

    // increment envelope stepper
    eic++;
    if(eic > samstep)
    {
        eic = 0;
        if(envelope_offset < 465)
            envelope_offset++;
    }

    return f;
}

void doSynth(Uint8 play)
{
    a_i1=0.f, a_i2=0.f, a_o1=0.f, a_o2=0.f;
    b_i1=0.f, b_i2=0.f, b_o1=0.f, b_o2=0.f;
    c_i1=0.f, c_i2=0.f, c_o1=0.f, c_o2=0.f;
    samstep = 0;
    eic = 0;
    crush_index = 0;
    crush_value = 0.f;
    crush_len = synth[selected_bank].dial_state[49] * dial_scale[49] * 33;
    envelope_offset = synth[selected_bank].dial_state[47] * dial_scale[47] * 466;
    for(int i = 0; i < 8; i++)
        oscphase[i] = 0.f;
    setSampleLen(synth[selected_bank].seclen);
    for(int i = 0; i < SAMPLE_RATE*synth[selected_bank].seclen; i++)
    {
        float s = 0.f;
        const float o8 = doOsc(8, 0.f, 0.f);
        const float o7 = doOsc(7, o8, 0.f);
        const float o4 = doOsc(4, o8, 0.f);
        const float o3 = doOsc(3, o7, o4);
        const float o6 = doOsc(6, o7, 0.f);
        const float o2 = doOsc(2, o6, o3);
        const float o5 = doOsc(5, o6, 0.f);
        const float o1 = doOsc(1, o5, o2);
        sample[i] = doFilters(o1);
    }
    
    if(play == 1)
        playSample();
}

struct sui
{
    Uint8 bankl_hover;
    Uint8 bankr_hover;
    Uint8 load_hover;
    Uint8 save_hover;
    Uint8 theme_hover;
    Uint8 secl_hover;
    Uint8 secr_hover;
    Uint8 export_hover;
    Uint8 play_hover;
    Uint8 dial_hover[50];
    Uint8 mul_hover[10];
    Uint8 fm_hover[10];
    Uint8 am_hover[10];
};
struct sui ui;

void loadAssets(SDL_Surface* screen)
{
    memset(&ui, 0x00, sizeof(struct sui));
    memset(&synth, 0x00, sizeof(struct ssynth));

    for(int i = 0; i < 256; i++)
        synth[i].seclen = 3;

    dial_rect[0] = (SDL_Rect){10, 19, 19, 19};
    dial_rect[1] = (SDL_Rect){34, 19, 19, 19};
    dial_rect[2] = (SDL_Rect){58, 19, 19, 19};
    dial_rect[3] = (SDL_Rect){82, 19, 19, 19};
    dial_rect[4] = (SDL_Rect){133, 19, 19, 19};
    dial_rect[5] = (SDL_Rect){157, 19, 19, 19};
    dial_rect[6] = (SDL_Rect){181, 19, 19, 19};
    dial_rect[7] = (SDL_Rect){205, 19, 19, 19};
    dial_rect[8] = (SDL_Rect){256, 19, 19, 19};
    dial_rect[9] = (SDL_Rect){280, 19, 19, 19};
    dial_rect[10] = (SDL_Rect){304, 19, 19, 19};
    dial_rect[11] = (SDL_Rect){328, 19, 19, 19};
    dial_rect[12] = (SDL_Rect){379, 19, 19, 19};
    dial_rect[13] = (SDL_Rect){403, 19, 19, 19};
    dial_rect[14] = (SDL_Rect){427, 19, 19, 19};
    dial_rect[15] = (SDL_Rect){451, 19, 19, 19};
    dial_rect[16] = (SDL_Rect){10, 75, 19, 19};
    dial_rect[17] = (SDL_Rect){34, 75, 19, 19};
    dial_rect[18] = (SDL_Rect){58, 75, 19, 19};
    dial_rect[19] = (SDL_Rect){82, 75, 19, 19};
    dial_rect[20] = (SDL_Rect){133, 75, 19, 19};
    dial_rect[21] = (SDL_Rect){157, 75, 19, 19};
    dial_rect[22] = (SDL_Rect){181, 75, 19, 19};
    dial_rect[23] = (SDL_Rect){205, 75, 19, 19};
    dial_rect[24] = (SDL_Rect){256, 75, 19, 19};
    dial_rect[25] = (SDL_Rect){280, 75, 19, 19};
    dial_rect[26] = (SDL_Rect){304, 75, 19, 19};
    dial_rect[27] = (SDL_Rect){328, 75, 19, 19};
    dial_rect[28] = (SDL_Rect){379, 75, 19, 19};
    dial_rect[29] = (SDL_Rect){403, 75, 19, 19};
    dial_rect[30] = (SDL_Rect){427, 75, 19, 19};
    dial_rect[31] = (SDL_Rect){451, 75, 19, 19};
    dial_rect[32] = (SDL_Rect){10, 119, 19, 19};
    dial_rect[33] = (SDL_Rect){34, 119, 19, 19};
    dial_rect[34] = (SDL_Rect){58, 119, 19, 19};
    dial_rect[35] = (SDL_Rect){82, 119, 19, 19};
    dial_rect[36] = (SDL_Rect){106, 119, 19, 19};
    dial_rect[37] = (SDL_Rect){136, 119, 19, 19};
    dial_rect[38] = (SDL_Rect){160, 119, 19, 19};
    dial_rect[39] = (SDL_Rect){184, 119, 19, 19};
    dial_rect[40] = (SDL_Rect){208, 119, 19, 19};
    dial_rect[41] = (SDL_Rect){232, 119, 19, 19};
    dial_rect[42] = (SDL_Rect){262, 119, 19, 19};
    dial_rect[43] = (SDL_Rect){286, 119, 19, 19};
    dial_rect[44] = (SDL_Rect){310, 119, 19, 19};
    dial_rect[45] = (SDL_Rect){334, 119, 19, 19};
    dial_rect[46] = (SDL_Rect){358, 119, 19, 19};
    dial_rect[47] = (SDL_Rect){388, 119, 19, 19};
    dial_rect[48] = (SDL_Rect){412, 119, 19, 19};
    dial_rect[49] = (SDL_Rect){447, 124, 19, 19};

    am_rect[0]  = (SDL_Rect){386, 55, 20, 12};
    mul_rect[0] = (SDL_Rect){419, 41, 12, 12};
    fm_rect[0]  = (SDL_Rect){443, 55, 20, 12};

    am_rect[1]  = (SDL_Rect){354, 14, 20, 12};
    mul_rect[1] = (SDL_Rect){361, 27, 12, 12};
    fm_rect[1]  = (SDL_Rect){354, 40, 20, 12};

    am_rect[2]  = (SDL_Rect){354, 70, 20, 12};
    mul_rect[2] = (SDL_Rect){361, 83, 12, 12};
    fm_rect[2]  = (SDL_Rect){354, 96, 20, 12};

    am_rect[3]  = (SDL_Rect){263, 55, 20, 12};
    mul_rect[3] = (SDL_Rect){296, 41, 12, 12};
    fm_rect[3]  = (SDL_Rect){320, 55, 20, 12};

    am_rect[4]  = (SDL_Rect){231, 14, 20, 12};
    mul_rect[4] = (SDL_Rect){238, 27, 12, 12};
    fm_rect[4]  = (SDL_Rect){231, 40, 20, 12};

    am_rect[5]  = (SDL_Rect){231, 70, 20, 12};
    mul_rect[5] = (SDL_Rect){238, 83, 12, 12};
    fm_rect[5]  = (SDL_Rect){231, 96, 20, 12};

    am_rect[6]  = (SDL_Rect){140, 55, 20, 12};
    mul_rect[6] = (SDL_Rect){173, 41, 12, 12};
    fm_rect[6]  = (SDL_Rect){197, 55, 20, 12};

    am_rect[7]  = (SDL_Rect){108, 14, 20, 12};
    mul_rect[7] = (SDL_Rect){115, 27, 12, 12};
    fm_rect[7]  = (SDL_Rect){108, 40, 20, 12};

    am_rect[8]  = (SDL_Rect){108, 70, 20, 12};
    mul_rect[8] = (SDL_Rect){115, 83, 12, 12};
    fm_rect[8]  = (SDL_Rect){108, 96, 20, 12};

    am_rect[9]  = (SDL_Rect){17, 55, 20, 12};
    mul_rect[9] = (SDL_Rect){50, 41, 12, 12};
    fm_rect[9]  = (SDL_Rect){74, 55, 20, 12};

    bb = SDL_RGBA32Surface(screen->w, screen->h);
    s_bg = surfaceFromData(&bmp_bg[0], screen->w, screen->h);
    s_icon = surfaceFromData((Uint32*)&icon_image.pixel_data[0], 16, 16);
    s_rm = surfaceFromData((Uint32*)&rm_image.pixel_data[0], 20, 12);
    s_tm = surfaceFromData((Uint32*)&tm_image.pixel_data[0], 20, 12);
    s_plus = surfaceFromData((Uint32*)&plus_image.pixel_data[0], 12, 12);
    s_minus = surfaceFromData((Uint32*)&minus_image.pixel_data[0], 12, 12);
    s_mul = surfaceFromData((Uint32*)&mul_image.pixel_data[0], 12, 12);
}

void render(SDL_Surface* screen)
{
    Uint32 ih = 0; // is hover
    char val[256]; // temporary string buffer

    // blit bg
    SDL_BlitSurface(s_bg, NULL, bb, NULL);

    // draw bank selection
    if(selected_bank < 10)
    {
        sprintf(val, "%d", selected_bank);
        drawText(bb, val, 39, 418, 0);
    }
    else if(selected_bank < 100)
    {
        sprintf(val, "%d", selected_bank);
        drawText(bb, val, 36, 418, 0);
    }
    else
    {
        sprintf(val, "%d", selected_bank);
        drawText(bb, val, 33, 418, 0);
    }

    // draw sec len
    if(synth[selected_bank].seclen < 10)
    {
        sprintf(val, "%d Sec", synth[selected_bank].seclen);
        drawText(bb, val, 327, 418, 0);
    }
    else if(synth[selected_bank].seclen < 100)
    {
        sprintf(val, "%d Sec", synth[selected_bank].seclen);
        drawText(bb, val, 325, 418, 0);
    }

    // envelope & oscilloscope
    Uint32 lx=7, ly=349;
    for(int i = 0; i < 466; i++)
    {
        // oscilloscope (63px flux) mid 349
        const float sc = ((float)synth[selected_bank].seclen * (float)SAMPLE_RATE) / 466.f;
        const Uint32 i2 = ((float)i)*sc;
        const Uint32 nx = 7+i;

        // aliased
        const Uint32 sa = (sample[i2]/2);

        // anti-aliased (sounds like a good idea, but not a good idea.)
        // float sa = 0.f;
        // if(i2 > 0)
        // {
        //     for(int j = 0; j < sc; j++)
        //         sa += sample[i2-j];
        // }
        // sa /= sc;
        // sa *= 0.496062994f; // 63/127

        // draw scope
        const Uint32 ny = 349+sa;
        line(bb, lx, ly, nx, ny, 220, 95, 117);
        lx = nx, ly = ny;

        // envelope
        line(bb, 7+i, 280, 7+i, 280-(126*synth[selected_bank].envelope[i]), 220, 95, 117);
    }
    
    if(envelope_enabled == 1)
        setColourLightness(bb, envelope_rect, scopecolor, 33);

    // dial hover & state
    for(int i = 0; i < 50; i++)
    {
        const Uint32 hh = dial_rect[i].h/2; // no point making this static
        const Uint32 sx = dial_rect[i].x+hh;
        const Uint32 sy = dial_rect[i].y+hh;
        const float radius = (float)(hh-2);
        const float c1 = (cos(1.570796371f + (synth[selected_bank].dial_state[i] * 6.283185482f)) * radius)+0.5f;
        const float s1 = (sin(1.570796371f + (synth[selected_bank].dial_state[i] * 6.283185482f)) * radius)+0.5f;
        const Uint32 ex = sx + c1;
        const Uint32 ey = sy + s1;
        line(bb, sx, sy, ex, ey, 255,255,255);
        //dda_line(bb, sx, sy, ex, ey, 0xFFFFFFFF);

        if(ui.dial_hover[i] == 1)
        {
            ih=1;
            
            sprintf(val, "Value: %+.2f", synth[selected_bank].dial_state[i] * dial_scale[i]);
            drawText(bb, val, 11, 397, 1);

            if(synth[selected_bank].select_mode == 0)
                setColourLightness(bb, dial_rect[i], scopecolor, select_lightness);
            else if(synth[selected_bank].select_mode == 1)
                replaceColour(bb, dial_rect[i], scopecolor, 117, 188, 99);
            else if(synth[selected_bank].select_mode == 2)
                replaceColour(bb, dial_rect[i], scopecolor, 220, 95, 117);
        }
    }

    // am hover & state
    for(int i = 0; i < 10; i++)
    {
        if(synth[selected_bank].am_state[i] == 1)
        {
            replaceColour(bb, am_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].am_state[i] == 2)
        {
            SDL_BlitSurface(s_rm, NULL, bb, &am_rect[i]);
            replaceColour(bb, am_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].am_state[i] == 3)
        {
            SDL_BlitSurface(s_tm, NULL, bb, &am_rect[i]);
            replaceColour(bb, am_rect[i], bgcolor, 117, 188, 99);
        }

        if(ui.am_hover[i] == 1)
        {
            ih=1;
            setAreaLightness(bb, am_rect[i], select_lightness);
        }
    }

    // mul hover & state
    for(int i = 0; i < 10; i++)
    {
        if(synth[selected_bank].mul_state[i] == 1)
        {
            SDL_BlitSurface(s_plus, NULL, bb, &mul_rect[i]);
            replaceColour(bb, mul_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].mul_state[i] == 2)
        {
            SDL_BlitSurface(s_minus, NULL, bb, &mul_rect[i]);
            replaceColour(bb, mul_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].mul_state[i] == 3)
        {
            SDL_BlitSurface(s_mul, NULL, bb, &mul_rect[i]);
            replaceColour(bb, mul_rect[i], bgcolor, 117, 188, 99);
        }
        
        if(ui.mul_hover[i] == 1)
        {
            ih=1;
            setAreaLightness(bb, mul_rect[i], select_lightness);
        }
    }

    // fm hover & state
    for(int i = 0; i < 10; i++)
    {
        if(synth[selected_bank].fm_state[i] == 1)
        {
            replaceColour(bb, fm_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].fm_state[i] == 2)
        {
            SDL_BlitSurface(s_tm, NULL, bb, &fm_rect[i]);
            replaceColour(bb, fm_rect[i], bgcolor, 117, 188, 99);
        }
        else if(synth[selected_bank].fm_state[i] == 3)
        {
            SDL_BlitSurface(s_rm, NULL, bb, &fm_rect[i]);
            replaceColour(bb, fm_rect[i], bgcolor, 117, 188, 99);
        }

        if(ui.fm_hover[i] == 1)
        {
            ih=1;
            setAreaLightness(bb, fm_rect[i], select_lightness);
        }
    }

    // button hover
    if(ui.bankl_hover == 1){ih=1; setColourLightness(bb, bankl_rect, bgcolor, select_lightness);}
    else if(ui.bankr_hover == 1){ih=1; setColourLightness(bb, bankr_rect, bgcolor, select_lightness);}
    else if(ui.load_hover == 1){ih=1; setColourLightness(bb, load_rect, bgcolor, select_lightness);}
    else if(ui.save_hover == 1){ih=1; setColourLightness(bb, save_rect, bgcolor, select_lightness);}
    else if(ui.theme_hover == 1){ih=1; setColourLightness(bb, theme_rect, bgcolor, select_lightness);}
    else if(ui.secl_hover == 1){ih=1; setColourLightness(bb, secl_rect, bgcolor, select_lightness);}
    else if(ui.secr_hover == 1){ih=1; setColourLightness(bb, secr_rect, bgcolor, select_lightness);}
    else if(ui.export_hover == 1){ih=1; setColourLightness(bb, export_rect, bgcolor, select_lightness);}
    else if(ui.play_hover == 1){ih=1; setColourLightness(bb, play_rect, bgcolor, select_lightness);}

    // colourise theme / novelty sdl per-pixel gfx benchmarking
    if(themeon > 0)
    {
        static Uint32 st = 0;
        static char tval[256];
        if(themeon == 2)
        {
            drawText(bb, tval, 319, 288, 1); // 321, 288 :: 145, 329
            if(SDL_GetTicks() > st)
            {
                st = 0;
                themeon = 0;
            }
        }
        else
        {
            if(st == 0)
                st = SDL_GetTicks();
            static float h = 0, s = 0.1f;
            if(theme_type == 2)
            {
                setHueSat(bb, export_rect, h, 0.5f);
                h += 0.002f;
            }
            else
            {
                setHueSat(bb, (SDL_Rect){0, 0, bb->w, bb->h}, h, s);
                h += 0.002f;
            }
            
            if(h >= 1.0f)
            {
                if(theme_type == 2)
                {
                    s = 0.1f;
                    h = 0.f;
                    themeon = 0;
                }
                else
                {
                    s += 0.1f;
                }
                h = 0.f;
            }
            else if(s >= 0.9f)
            {
                s = 0.1f;
                h = 0.f;
                themeon = 2;
                sprintf(tval, "Time Taken: %.2f seconds", ((float)(SDL_GetTicks() - st))/1000.f);
                st = SDL_GetTicks()+9000;
            }
        }
    }

    // blit to screen
    SDL_BlitSurface(bb, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);

    // cursor
    if(ih == 1)
        SDL_CursorPointer(1);
    else
        SDL_CursorPointer(0);
}

int main(int argc, char *args[])
{
    // init sdl
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "ERROR: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // create window
    window = SDL_CreateWindow("Borg ER-3 - ALPHA 0.4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_rect.w, screen_rect.h, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        fprintf(stderr, "ERROR: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    // create screen
    SDL_Surface* screen = SDL_GetWindowSurface(window);

    // get app dir
    basedir = SDL_GetBasePath();
    appdir = SDL_GetPrefPath("voxdsp", "borger3");
    printf("%s\n", basedir);
    printf("%s\n", appdir);

    // load assets
    loadAssets(screen);

    // load bank
    loadState();

    //init audio
    initMonoAudio(SAMPLE_RATE);

    // test sample
    // setSampleLen(33);
    // for(int i = 0; i < SAMPLE_RATE*33; i++)
    // {
    //     const float p = (Hz(1)/SAMPLE_RATE) * (((float)i));
    //     sample[i] = getImpulse(p, 60.f)*63.f; //sin(p)*60.f; //rand()%5;
    // }
    //playSample();

    // themeing (this is the general idea of how to re-theme, I don't intend to use this in production)
    // but say someone made a modification, it might be a good idea to change the theme a little
    // so that users know which version they are using.
    // ----------------------------------------------
    // replaceColour(s_bg, screen_rect, outlinecolor, 0, 200, 0);
    // outlinecolor = getpixel(s_bg, 6, 14);
    // replaceColour(s_bg, screen_rect, bgcolor, 32, 77, 133);
    // bgcolor = getpixel(s_bg, 0, 0);
    // replaceColour(s_bg, screen_rect, scopecolor, 77, 133, 0);
    // scopecolor = getpixel(s_bg, 7, 155);

    // set icon
    SDL_SetWindowIcon(window, s_icon);

    // first render
    doSynth(0);
    render(screen);

    // event loop
    while(1)
    {
        SDL_Event event;
        while(SDL_WaitEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                {
                    render(screen);
                }
                break;

                case SDL_MOUSEMOTION:
                {
                    const Sint32 x = event.motion.x, y = event.motion.y;
                    static struct sui lui;
                    
                    // Adjust envelope
                    if(envelope_enabled == 1 && x > 6 && x < 473 && y > 155 && y < 282)
                    {
                        static Uint32 li = 0;
                        const Uint32 ni = x-7;
                        const float nv = 1.f-((float)(y-155) * 0.007936508395f);
                        synth[selected_bank].envelope[ni] = nv;

                        // really simple but effective smoothing method
                        // otherwise the input code needs to be executed on
                        // a seperate thread where the render no longer blocks.
                        Uint32 di = ni-li;
                        if(di < 6)
                        {
                            while(di > 0)
                            {
                                synth[selected_bank].envelope[li+di] = nv;
                                di--;
                            }
                        }
                        
                        // tick envalope turn renders at 20 fps
                        static Uint32 lt = 0;
                        if(SDL_GetTicks() > lt)
                        {
                            render(screen);
                            lt = SDL_GetTicks() + 50;
                        }

                        li = ni;
                        break;
                    }

                    // render dial rotations or button hovers
                    if(selected_dial >= 0)
                    {
                        const Sint32 hh = dial_rect[selected_dial].h/2; // no point making this static

                        // scale dial sensitivity
                        float sense = 0.001 / dial_scale[selected_dial];
                        if(synth[selected_bank].select_mode == 0)
                        {
                            if(dial_scale[selected_dial] == MAXFREQUENCY)
                                sense = 0.3 / dial_scale[selected_dial];
                            else if(dial_scale[selected_dial] == MAXAMPLITUDE)
                                sense = 0.1 / dial_scale[selected_dial];
                            else if(dial_scale[selected_dial] == MAXRESOLUTION)
                                sense = 0.1 / dial_scale[selected_dial];
                        }
                        else if(synth[selected_bank].select_mode == 1)
                            sense = 0.001 / dial_scale[selected_dial];
                        else if(synth[selected_bank].select_mode == 2)
                            sense = 0.3 / dial_scale[selected_dial];

                        // do rotation
                        if(dial_neg[selected_dial] == 1)
                        {
                            synth[selected_bank].dial_state[selected_dial] += ((dial_rect[selected_dial].y+hh) - y)*sense;
                            if(synth[selected_bank].dial_state[selected_dial] >= 1.f)
                                synth[selected_bank].dial_state[selected_dial] = 1.f;
                            else if(synth[selected_bank].dial_state[selected_dial] < -1.f)
                                synth[selected_bank].dial_state[selected_dial] = -1.f;
                        }
                        else
                        {
                            synth[selected_bank].dial_state[selected_dial] += ((dial_rect[selected_dial].y+hh) - y)*sense;
                            if(synth[selected_bank].dial_state[selected_dial] >= 1.f)
                                synth[selected_bank].dial_state[selected_dial] = 1.f;
                            else if(synth[selected_bank].dial_state[selected_dial] < 0.f)
                                synth[selected_bank].dial_state[selected_dial] = 0.f;
                        }
                        SDL_WarpMouseInWindow(window, dial_rect[selected_dial].x+hh, dial_rect[selected_dial].y+hh);

                        // tick dial turn renders at 20 fps
                        static Uint32 lt = 0;
                        if(SDL_GetTicks() > lt)
                        {
                            render(screen);
                            lt = SDL_GetTicks() + 50;
                        }
                    }
                    else
                    {
                        // clear states
                        memset(&ui, 0x00, sizeof(struct sui));

                        // check for hover state and return
                        Uint8 sc = 0; // skip check
                        if(inrange(&ui.bankl_hover, &sc, x, y, bankl_rect) == 0)
                        if(inrange(&ui.bankr_hover, &sc, x, y, bankr_rect) == 0)
                        if(inrange(&ui.load_hover, &sc, x, y, load_rect) == 0)
                        if(inrange(&ui.save_hover, &sc, x, y, save_rect) == 0)
                        if(inrange(&ui.theme_hover, &sc, x, y, theme_rect) == 0)
                        if(inrange(&ui.secl_hover, &sc, x, y, secl_rect) == 0)
                        if(inrange(&ui.secr_hover, &sc, x, y, secr_rect) == 0)
                        if(inrange(&ui.export_hover, &sc, x, y, export_rect) == 0)
                        if(inrange(&ui.play_hover, &sc, x, y, play_rect) == 0)
                        for(int i = 0; i < 50; i++)
                            if(inrange(&ui.dial_hover[i], &sc, x, y, dial_rect[i]) == 1)
                                break;
                        if(sc == 0)
                            for(int i = 0; i < 10; i++)
                                if(inrange(&ui.am_hover[i], &sc, x, y, am_rect[i]) == 1)
                                    break;
                        if(sc == 0)
                            for(int i = 0; i < 10; i++)
                                if(inrange(&ui.mul_hover[i], &sc, x, y, mul_rect[i]) == 1)
                                    break;
                        if(sc == 0)
                            for(int i = 0; i < 10; i++)
                                if(inrange(&ui.fm_hover[i], &sc, x, y, fm_rect[i]) == 1)
                                    break;

                        // render if there has been a change in state
                        if(memcmp(&lui, &ui, sizeof(struct sui)) != 0)
                        {
                            render(screen);
                            memcpy(&lui, &ui, sizeof(struct sui));
                        }
                    }
                }
                break;

                case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        doSynth(1);
                        render(screen);
                    }
                }
                break;

                case SDL_MOUSEBUTTONUP:
                {
                    static struct ssynth lsyn[256];

                    if(envelope_enabled == 1)
                    {
                        envelope_enabled = 0;
                        doSynth(0);
                        render(screen);
                        memcpy(&lsyn, &synth[selected_bank], sizeof(struct sui));
                    }
                    else if(selected_dial >= 0)
                    {
                        SDL_ShowCursor(1);
                        selected_dial = -1;
                        doSynth(0);
                        render(screen);
                        memcpy(&lsyn, &synth[selected_bank], sizeof(struct sui));
                    }
                    else
                    {
                        if(memcmp(&lsyn, &synth[selected_bank], sizeof(struct sui)) != 0)
                        {
                            doSynth(0);
                            render(screen);
                            memcpy(&lsyn, &synth[selected_bank], sizeof(struct sui));
                        }
                    }
                }
                break;

                case SDL_MOUSEBUTTONDOWN:
                {
                    const Uint32 x = event.button.x, y = event.button.y;

                    if(event.button.button == SDL_BUTTON_RIGHT)
                    {
                        synth[selected_bank].select_mode++;
                        if(synth[selected_bank].select_mode >= 3)
                            synth[selected_bank].select_mode = 0;
                        
                        render(screen);
                    }

                    if(event.button.button == SDL_BUTTON_LEFT)
                    {
                        Uint8 sc = 0; // skip check

                        for(int i = 0; i < 50; i++)
                        {
                            if(ui.dial_hover[i] == 1)
                            {
                                SDL_ShowCursor(0);
                                selected_dial = i;
                                break;
                            }
                        }

                        if(sc == 0)
                        {
                            if(x > 6 && x < 473 && y > 155 && y < 282)
                            {
                                sc=1;
                                envelope_enabled = 1;
                            }
                            else if(ui.play_hover == 1)
                            {
                                sc=1;
                                doSynth(1);
                            }
                            else if(ui.export_hover == 1)
                            {
                                sc=1;
                                char file[256];
                                sprintf(file, "%sbank-%d.wav", basedir, selected_bank);
                                writeWAV(file);

                                // some user feedback
                                theme_type = 2;
                                themeon=1;
                                while(themeon == 1)
                                    render(screen);
                            }
                            else if(ui.load_hover == 1)
                            {
                                sc=1;
                                loadState();
                            }
                            else if(ui.save_hover == 1)
                            {
                                sc=1;
                                saveState();
                            }
                            else if(ui.bankl_hover == 1)
                            {
                                stopSample();
                                sc=1;
                                selected_bank--;
                                doSynth(0);
                                render(screen);
                            }
                            else if(ui.bankr_hover == 1)
                            {
                                stopSample();
                                sc=1;
                                selected_bank++;
                                doSynth(0);
                                render(screen);
                            }
                            else if(ui.secl_hover == 1)
                            {
                                sc=1;
                                synth[selected_bank].seclen--;
                                if(synth[selected_bank].seclen == 0)
                                    synth[selected_bank].seclen = MAXSAMPLELEN;
                                doSynth(0);
                                stopSample();
                            }
                            else if(ui.secr_hover == 1)
                            {
                                sc=1;
                                synth[selected_bank].seclen++;
                                if(synth[selected_bank].seclen > MAXSAMPLELEN)
                                    synth[selected_bank].seclen = 1;
                                doSynth(0);
                                stopSample();
                            }
                        }
                        
                        if(sc == 0)
                        {
                            for(int i = 0; i < 10; i++)
                            {
                                if(ui.am_hover[i] == 1)
                                {
                                    sc=1;
                                    synth[selected_bank].am_state[i]++;
                                    if(synth[selected_bank].am_state[i] >= 4)
                                        synth[selected_bank].am_state[i] = 0;
                                    break;
                                }
                            }
                        }

                        if(sc == 0)
                        {
                            for(int i = 0; i < 10; i++)
                            {
                                if(ui.mul_hover[i] == 1)
                                {
                                    sc=1;
                                    synth[selected_bank].mul_state[i]++;
                                    if(synth[selected_bank].mul_state[i] >= 4)
                                        synth[selected_bank].mul_state[i] = 0;
                                    break;
                                }
                            }
                        }

                        if(sc == 0)
                        {
                            for(int i = 0; i < 10; i++)
                            {
                                if(ui.fm_hover[i] == 1)
                                {
                                    sc=1;
                                    synth[selected_bank].fm_state[i]++;
                                    if(synth[selected_bank].fm_state[i] >= 4)
                                        synth[selected_bank].fm_state[i] = 0;
                                    break;
                                }
                            }
                        }

                        if(sc == 0)
                        {
                            if(ui.theme_hover == 1)
                            {
                                sc=1;
                                theme_type=1;
                                themeon=1;
                                while(themeon == 1)
                                    render(screen);
                            }
                        }

                        render(screen);
                    }
                }
                break;
                
                case SDL_QUIT:
                {
                    saveState();
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    exit(0);
                }
                break;
            }
        }
    }

    //Done.
    return 0;
}

