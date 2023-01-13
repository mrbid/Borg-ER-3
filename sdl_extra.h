/*
    James William Fletcher (github.com/mrbid)
        September 2021
    
    A few SDL helper functions to make life
    a little bit easier with little overhead.
*/
#ifndef SDL_EXTRA_H
#define SDL_EXTRA_H

#include <math.h>
#include <SDL2/SDL.h>

// c helper
float minf(float x, float y);
float maxf(float x, float y);
float wrapf(float f);

// sdl helper
Uint8 inrangel(Uint32 x, Uint32 y, SDL_Rect r);
Uint8 inrange( Uint8* out, Uint8* skip_check, Uint32 x, Uint32 y, SDL_Rect r);
void  SDL_KeyGreenToAlpha(SDL_Surface* image);
void  SDL_CursorPointer(Uint32 type); // pass 1337 to cleanup
SDL_Surface* SDL_RGBA32Surface(Uint32 w, Uint32 h);

// font
void drawText(SDL_Surface* surface, const char* s, Uint32 x, Uint32 y, Uint8 colour);
// to cleanup / destroy buffers: drawText(NULL, "*K", 0, 0, 0);
// colours: 0 = black, 1 = white (use replaceColour for extended colour sets)

// set/get pixel for all (source: SDL wiki ~2006)
Uint32 SDL_GetPixel(const SDL_Surface *surface, Uint32 x, Uint32 y);
void   SDL_SetPixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 colour);

// blitting
void SDL_OpacityBlit(const SDL_Surface* surface_in, SDL_Surface* surface_out, Uint32 x, Uint32 y, float opacity);
void SDL_FillRectAlpha(SDL_Surface* surface, const SDL_Rect* r, Uint32 colour, float alpha);

// set/get pixel for RGBA32
Uint32 getpixel(const SDL_Surface *surface, Uint32 x, Uint32 y);
void   setpixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 colour);
void   setpixelrgb(SDL_Surface *surface, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b);
void   setpixelrgba(SDL_Surface *surface, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// primitive drawing
void line(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, Uint32 colour);
void linergb(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, Uint8 r, Uint8 g, Uint8 b);
void dda_line(SDL_Surface* surface, Uint32 x1, Uint32 y1, Uint32 x2, Uint32 y2, Uint32 colour);
void circle(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 radius, Uint32 colour);
void circlergb(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 radius, Uint8 r, Uint8 g, Uint8 b);
void fillcircle(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 radius, Uint32 colour);
void fillcirclergb(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 radius, Uint8 r, Uint8 g, Uint8 b);

// colour transformations
void colourisePixel(SDL_Surface* surface, Uint32 x, Uint32 y, float hue, float saturation);
void setHueSat(SDL_Surface* surface, SDL_Rect r, float hue, float saturation);
void replaceColour(SDL_Surface* surface, SDL_Rect r, Uint32 colour, Uint8 r8, Uint8 g8, Uint8 b8);
void setColourLightness(SDL_Surface* surface, SDL_Rect r, Uint32 colour, Uint8 light);
void setColourDarkness(SDL_Surface* surface, SDL_Rect r, Uint32 colour, Uint8 dark);
void setAreaLightness(SDL_Surface* surface, SDL_Rect r, Uint8 light);
void setAreaDarkness(SDL_Surface* surface, SDL_Rect r, Uint8 dark);

// surface conversion
void bmpToArray(const char* file); // gimp export to .c is generally a better option
void dataToSurface(SDL_Surface* surface, const Uint32* data, Uint32 w, Uint32 h);
SDL_Surface* surfaceFromData(const Uint32* data, Uint32 w, Uint32 h);


/*
    Fun-ctions
                    .'"".
                   c' )"/     - Shanaka Dias
            _____  _>__/_ ______
           /..  .'`      `'. .':B
          8;  ' .       .  ' .'|
          \ '...'\  __ / '..' _/
           |      : _ _:_    /
            \      (     )  /
            \_   .'  '  : __/
              \_:_      __/
                //:____/\\
             __|/       |/__
            ;_._)       (,__;
*/

Uint32 SDL_GetPixel(const SDL_Surface *surface, Uint32 x, Uint32 y)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1){return 0;}

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        default:
            return 0;
    }
}

void SDL_SetPixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pixel)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1){return;}

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

void SDL_KeyGreenToAlpha(SDL_Surface* image)
{
    SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 0, 255, 0));
}

SDL_Surface* SDL_RGBA32Surface(Uint32 w, Uint32 h)
{
    return SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
}

void SDL_CursorPointer(Uint32 type)
{
    static Uint32 init = 0;
    static SDL_Cursor* cursor_hand;
    static SDL_Cursor* cursor_arrow;

    if(init == 0)
    {
        cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        cursor_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        init = 1;
    }

    if(type == 0)
        SDL_SetCursor(cursor_arrow);
    else if(type == 1)
        SDL_SetCursor(cursor_hand);
    else if(type == 1337)
    {
        SDL_FreeCursor(cursor_hand);
        SDL_FreeCursor(cursor_arrow);
    }
}

Uint8 inrangel(Uint32 x, Uint32 y, SDL_Rect r)
{
    if(x >= r.x && y >= r.y && x <= r.x+r.w && y <= r.y+r.h)
        return 1;
    return 0;
}

Uint8 inrange(Uint8* out, Uint8* skip_check, Uint32 x, Uint32 y, SDL_Rect r)
{
    if(x >= r.x && y >= r.y && x <= r.x+r.w && y <= r.y+r.h)
    {
        *skip_check = 1;
        *out = 1;
        return 1;
    }
    *out = 0;
    return 0;
}

float minf(float x, float y)
{
    return x < y ? x : y;
}

float maxf(float x, float y)
{
    return x > y ? x : y;
}

float wrapf(float f)
{
    if(f < 0.f){return f + 1.0f;}
    if(f > 1.f){return f - 1.0f;}
    return f;
}

Uint32 getpixel(const SDL_Surface *surface, Uint32 x, Uint32 y)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1 || surface->format->BytesPerPixel != 4){return 0;}
    const Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    return *(Uint32*)p;
}

void setpixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pix)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1 || surface->format->BytesPerPixel != 4){return;}
    const Uint8* pixel = (Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);
    *((Uint32*)pixel) = pix;
}

void setpixelrgb(SDL_Surface *surface, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1 || surface->format->BytesPerPixel != 4){return;}
    const Uint8* pixel = (Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);
    *((Uint32*)pixel) = SDL_MapRGB(surface->format, r, g, b);
}

void setpixelrgba(SDL_Surface *surface, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if(x < 0 || x > surface->w-1 || y < 0 || y > surface->h-1 || surface->format->BytesPerPixel != 4){return;}
    const Uint8* pixel = (Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);
    *((Uint32*)pixel) = SDL_MapRGBA(surface->format, r, g, b, a);
}

// could be more efficient: https://stackoverflow.com/a/64090995
void colourisePixel(SDL_Surface* surf, Uint32 x, Uint32 y, float h, float s)
{
    Uint8 r8, g8, b8, a8;
    SDL_GetRGBA(getpixel(surf, x, y), surf->format, &r8, &g8, &b8, &a8);
    const float nr = ((float)(r8))*0.003921569f, ng = ((float)(g8))*0.003921569f, nb = ((float)(b8))*0.003921569f;
    const float minc = minf(minf(nr, ng), nb);
    const float maxc = maxf(maxf(nr, ng), nb);
    const float l = (maxc + minc)*0.5f;

    //if(r8+g8+b8 < 100){a8=0;}
    
    if(s <= 0){setpixel(surf, x, y, SDL_MapRGBA(surf->format, l*255.f, l*255.f, l*255.f, a8));}

    const float t2 = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
    const float t1 = 2.0f * l - t2;

    const float rt3 = wrapf(h + 0.3333333433f);
    const float gt3 = wrapf(h);
    const float bt3 = wrapf(h - 0.3333333433f);

    const float r = (6.0f * rt3 < 1.f) ? t1 + (t2-t1) * 6.0f * rt3 : (2.0f * rt3 < 1) ? t2 : (3.0f * rt3 < 2) ? t1 + (t2-t1) * (0.6666666865f - rt3) * 6.0f : t1;
    const float g = (6.0f * gt3 < 1.f) ? t1 + (t2-t1) * 6.0f * gt3 : (2.0f * gt3 < 1) ? t2 : (3.0f * gt3 < 2) ? t1 + (t2-t1) * (0.6666666865f - gt3) * 6.0f : t1;
    const float b = (6.0f * bt3 < 1.f) ? t1 + (t2-t1) * 6.0f * bt3 : (2.0f * bt3 < 1) ? t2 : (3.0f * bt3 < 2) ? t1 + (t2-t1) * (0.6666666865f - bt3) * 6.0f : t1;

    setpixel(surf, x, y, SDL_MapRGBA(surf->format, (Uint8)(r*255.f), (Uint8)(g*255.f), (Uint8)(b*255.f), a8));
}

void line(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, Uint32 colour)
{
    const int dx = abs((Sint32)x1 - (Sint32)x0), sx = x0 < x1 ? 1 : -1;
    const int dy = abs((Sint32)y1 - (Sint32)y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while(1)
    {
        setpixel(surface, x0, y0, colour);
        if(x0 == x1 && y0 == y1)
            break;

        e2 = err;
        if(e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}
void linergb(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, Uint8 r, Uint8 g, Uint8 b)
{
    const Uint32 clr = SDL_MapRGB(surface->format, r,g,b);
    line(surface, x0, y0, x1, y1, clr);
}

void dda_line(SDL_Surface* surf, Uint32 x1, Uint32 y1, Uint32 x2, Uint32 y2, Uint32 colour)
{
    float x = x1, y = y1;
    const float dx = (Sint32)x2-(Sint32)x1;
    const float dy = (Sint32)y2-(Sint32)y1;
    const float fdx = fabs(dx);
    const float fdy = fabs(dy);
    const float mr = fdx >= fdy ? 1.f/fdx : 1.f/fdy;
    const float m = fdx >= fdy ? fdx : fdy;
    setpixel(surf, x1, y1, colour);
    for(Uint32 i = 1; i <= m; i++)
    {
        x = x+(dx*mr);
        y = y+(dy*mr);
        setpixel(surf, (Uint32)x, (Uint32)y, colour);
    }
}

// blits an image with specified opactiy
void SDL_OpacityBlit(const SDL_Surface* in, SDL_Surface* out, Uint32 ix, Uint32 iy, float opacity)
{
    static Uint32 colourkey = 0;
    if(colourkey == 0)
        colourkey = SDL_MapRGB(in->format, 0, 255, 0);

    for(Uint32 y = 0; y < in->h; y++)
    {
        for(Uint32 x = 0; x < in->w; x++)
        {
            const Uint32 p1 = SDL_GetPixel(in, x, y);
            if(p1 == colourkey){continue;}
            Uint8 r1, g1, b1;
            SDL_GetRGB(p1, in->format, &r1, &g1, &b1);

            const Uint32 p2 = SDL_GetPixel(out, ix+x, iy+y);
            Uint8 r2, g2, b2;
            SDL_GetRGB(p2, out->format, &r2, &g2, &b2);

            const Uint8 rn = (1 - opacity) * r1 + (opacity * r2);
            const Uint8 gn = (1 - opacity) * g1 + (opacity * g2);
            const Uint8 bn = (1 - opacity) * b1 + (opacity * b2);

            SDL_SetPixel(out, ix+x, iy+y, SDL_MapRGB(out->format, rn, gn, bn));
        }
    }
}


//Draws an alpha blended rectangle
void SDL_FillRectAlpha(SDL_Surface* surf, const SDL_Rect* r, Uint32 colour, float alpha)
{	 
    for(Uint32 x = r->x; x < r->x+r->w; x++)
    {
        for(Uint32 y = r->y; y < r->y+r->h; y++)
        {
            const Uint32 p1 = SDL_GetPixel(surf, x, y);

            Uint8 r1, g1, b1;
            SDL_GetRGB(p1, surf->format, &r1, &g1, &b1);

            Uint8 r2, g2, b2;
            SDL_GetRGB(colour, surf->format, &r2, &g2, &b2);

            const Uint8 rn = (1 - alpha) * r1 + (alpha * r2);
            const Uint8 gn = (1 - alpha) * g1 + (alpha * g2);
            const Uint8 bn = (1 - alpha) * b1 + (alpha * b2);

            SDL_SetPixel(surf, x, y, SDL_MapRGB(surf->format, rn, gn, bn));
        }
    }
}

// bresenham circle (this was taken from the mid-point circle wikipedia article some years ago, has since been long removed)
// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void circle(SDL_Surface* surf, Uint32 x0, Uint32 y0, Uint32 radius, Uint32 colour)
{
    Sint32 f = 1 - radius;
    Sint32 ddF_x = 1;
    Sint32 ddF_y = -2 * radius;
    Sint32 x = 0;
    Sint32 y = radius;

    setpixel(surf, x0, y0 + radius, colour);
    setpixel(surf, x0, y0 - radius, colour);
    setpixel(surf, x0 + radius, y0, colour);
    setpixel(surf, x0 - radius, y0, colour);
    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        setpixel(surf, x0 + x, y0 + y, colour);
        setpixel(surf, x0 - x, y0 + y, colour);
        setpixel(surf, x0 + x, y0 - y, colour);
        setpixel(surf, x0 - x, y0 - y, colour);
        setpixel(surf, x0 + y, y0 + x, colour);
        setpixel(surf, x0 - y, y0 + x, colour);
        setpixel(surf, x0 + y, y0 - x, colour);
        setpixel(surf, x0 - y, y0 - x, colour);
    }
}
void circlergb(SDL_Surface* surf, Uint32 x0, Uint32 y0, Uint32 radius, Uint8 r, Uint8 g, Uint8 b)
{
    const Uint32 clr = SDL_MapRGB(surf->format, r,g,b);
    circle(surf, x0, y0, radius, clr);
}

// this is an adaption of the bresenham circle
void fillcircle(SDL_Surface* surf, Uint32 x0, Uint32 y0, Uint32 radius, Uint32 colour)
{
    Sint32 f = 1 - radius;
    Sint32 ddF_x = 1;
    Sint32 ddF_y = -2 * radius;
    Sint32 x = 0;
    Sint32 y = radius;

    line(surf, x0 + radius, y0, x0 - radius, y0, colour);
    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        line(surf, x0 + x, y0 + y, x0 - x, y0 + y, colour);
        line(surf, x0 + x, y0 - y, x0 - x, y0 - y, colour);
        line(surf, x0 + y, y0 + x, x0 - y, y0 + x, colour);
        line(surf, x0 + y, y0 - x, x0 - y, y0 - x, colour);
    }
}
void fillcirclergb(SDL_Surface* surf, Uint32 x0, Uint32 y0, Uint32 radius, Uint8 r, Uint8 g, Uint8 b)
{
    const Uint32 clr  = SDL_MapRGB(surf->format, r,g,b);
    fillcircle(surf, x0, y0, radius, clr);
}

void bmpToArray(const char* file)
{
    SDL_Surface* s = SDL_LoadBMP(file);
    SDL_Surface* s2 = SDL_CreateRGBSurfaceWithFormat(0, s->w, s->h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_BlitSurface(s, NULL, s2, NULL);

    FILE* f = fopen("res.h", "a");
    if(f != NULL)
    {
        fprintf(f, "// %s\n", file);

        for(Uint32 y = 0; y < s->h; ++y)
        {
            for(Uint32 x = 0; x < s->w; ++x)
            {
                fprintf(f, "0x%X,", getpixel(s2, x, y));
            }
        }

        fprintf(f, "\n\n");
        fclose(f);
    }

    SDL_FreeSurface(s);
    SDL_FreeSurface(s2);
}

void dataToSurface(SDL_Surface* s, const Uint32* data, Uint32 w, Uint32 h)
{
    memcpy(s->pixels, data, s->pitch*h);
}

SDL_Surface* surfaceFromData(const Uint32* data, Uint32 w, Uint32 h)
{
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    memcpy(s->pixels, data, s->pitch*h);
    return s;
}

void setHueSat(SDL_Surface* surf, SDL_Rect r, float h, float s)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
        for(Uint32 x = r.x; x < max_x; ++x)
            colourisePixel(surf, x, y, h, s);
}

void replaceColour(SDL_Surface* surf, SDL_Rect r, Uint32 colour, Uint8 r8, Uint8 g8, Uint8 b8)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
    {
        for(Uint32 x = r.x; x < max_x; ++x)
        {
            const Uint32 p = getpixel(surf, x, y);
            if(p == colour)
                setpixelrgb(surf, x, y, r8, g8, b8);
        }
    }
}

void setColourLightness(SDL_Surface* surf, SDL_Rect r, Uint32 colour, Uint8 light)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
    {
        for(Uint32 x = r.x; x < max_x; ++x)
        {
            const Uint32 p = getpixel(surf, x, y);
            if(p == colour)
            {
                Uint8 r8, g8, b8;
                SDL_GetRGB(p, surf->format, &r8, &g8, &b8);
                if((Sint16)r8 + (Sint16)light < 255)
                    r8 += light;
                if((Sint16)g8 + (Sint16)light < 255)
                    g8 += light;
                if((Sint16)b8 + (Sint16)light < 255)
                    b8 += light;
                setpixelrgb(surf, x, y, r8, g8, b8);
            }
        }
    }
}

void setColourDarkness(SDL_Surface* surf, SDL_Rect r, Uint32 colour, Uint8 dark)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
    {
        for(Uint32 x = r.x; x < max_x; ++x)
        {
            const Uint32 p = getpixel(surf, x, y);
            if(p == colour)
            {
                Uint8 r8, g8, b8;
                SDL_GetRGB(p, surf->format, &r8, &g8, &b8);
                if((Sint16)r8 - (Sint16)dark > 0)
                    r8 -= dark;
                if((Sint16)g8 - (Sint16)dark > 0)
                    g8 -= dark;
                if((Sint16)b8 - (Sint16)dark > 0)
                    b8 -= dark;
                setpixelrgb(surf, x, y, r8, g8, b8);
            }
        }
    }
}

void setAreaLightness(SDL_Surface* surf, SDL_Rect r, Uint8 light)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
    {
        for(Uint32 x = r.x; x < max_x; ++x)
        {
            const Uint32 p = getpixel(surf, x, y);

            Uint8 r8, g8, b8;
            SDL_GetRGB(p, surf->format, &r8, &g8, &b8);
            if((Sint16)r8 + (Sint16)light < 255)
                r8 += light;
            if((Sint16)g8 + (Sint16)light < 255)
                g8 += light;
            if((Sint16)b8 + (Sint16)light < 255)
                b8 += light;
            setpixelrgb(surf, x, y, r8, g8, b8);
        }
    }
}

void setAreaDarkness(SDL_Surface* surf, SDL_Rect r, Uint8 dark)
{
    const Uint32 max_y = r.y+r.h;
    const Uint32 max_x = r.x+r.w;
    for(Uint32 y = r.y; y < max_y; ++y)
    {
        for(Uint32 x = r.x; x < max_x; ++x)
        {
            const Uint32 p = getpixel(surf, x, y);

            Uint8 r8, g8, b8;
            SDL_GetRGB(p, surf->format, &r8, &g8, &b8);
            if((Sint16)r8 - (Sint16)dark > 0)
                r8 -= dark;
            if((Sint16)g8 - (Sint16)dark > 0)
                g8 -= dark;
            if((Sint16)b8 - (Sint16)dark > 0)
                b8 -= dark;
            setpixelrgb(surf, x, y, r8, g8, b8);
        }
    }
}

const Uint8 font_map[] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\xFF\xFF\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\0\xFF\0\0\0\xFF\xFF\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\xFF\0\0\0\0\xFF\0\0\0\0\xFF\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\xFF\0\0\xFF\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\0\0\0\xFF\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\0\xFF\xFF\0\0\0\0\xFF\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\xFF\0\0\0\0\xFF\0\0\xFF\0\xFF\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\0\xFF\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\xFF\xFF\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\0\0\0\0\xFF\0\0\0\0\0\0\0\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\xFF\xFF\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\xFF\xFF\xFF\xFF\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\xFF\0\0\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\0\xFF\xFF\xFF\xFF\0\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\xFF\xFF\0\0\0\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\0\0\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\0\0\0\0\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\0\0\0\0\xFF\xFF\0\0\xFF\0\0\xFF\xFF\xFF\xFF\xFF\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
void drawText(SDL_Surface* o, const char* s, Uint32 x, Uint32 y, Uint8 colour)
{
    static const Uint32 m = 1;
    static SDL_Surface* font_black = NULL;
    static SDL_Surface* font_white = NULL;
    if(font_black == NULL)
    {
        font_black = SDL_RGBA32Surface(447, 11);
        for(int y = 0; y < font_black->h; y++)
        {
            for(int x = 0; x < font_black->w; x++)
            {
                const Uint8 c = font_map[(y*font_black->w)+x];
                setpixel(font_black, x, y, SDL_MapRGBA(font_black->format, c, c, c, 255-c));
            }
        }
        font_white = SDL_RGBA32Surface(447, 11);
        SDL_BlitSurface(font_black, &font_black->clip_rect, font_white, &font_white->clip_rect);
        replaceColour(font_white, font_white->clip_rect, 0xFF000000, 255, 255, 255);
    }
    if(s[0] == '*' && s[1] == 'K') // signal cleanup
    {
        SDL_FreeSurface(font_black);
        SDL_FreeSurface(font_white);
        font_black = NULL;
        return;
    }

    SDL_Surface* font = font_black;
    if(colour == 1)
        font = font_white;

    SDL_Rect dr = {x, y, 0, 0};

    const Uint32 len = strlen(s);
    for(Uint32 i = 0; i < len; i++)
    {
        if(s[i] == 'A'){SDL_BlitSurface(font, &(SDL_Rect){0,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'B'){SDL_BlitSurface(font, &(SDL_Rect){8,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'C'){SDL_BlitSurface(font, &(SDL_Rect){15,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'D'){SDL_BlitSurface(font, &(SDL_Rect){22,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'E'){SDL_BlitSurface(font, &(SDL_Rect){30,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 'F'){SDL_BlitSurface(font, &(SDL_Rect){36,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 'G'){SDL_BlitSurface(font, &(SDL_Rect){42,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'H'){SDL_BlitSurface(font, &(SDL_Rect){50,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'I'){SDL_BlitSurface(font, &(SDL_Rect){58,0,4,9}, o, &dr); dr.x += 4+m;}
        if(s[i] == 'J'){SDL_BlitSurface(font, &(SDL_Rect){63,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 'K'){SDL_BlitSurface(font, &(SDL_Rect){69,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'L'){SDL_BlitSurface(font, &(SDL_Rect){76,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 'M'){SDL_BlitSurface(font, &(SDL_Rect){82,0,9,9}, o, &dr); dr.x += 9+m;}
        if(s[i] == 'N'){SDL_BlitSurface(font, &(SDL_Rect){92,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'O'){SDL_BlitSurface(font, &(SDL_Rect){99,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'P'){SDL_BlitSurface(font, &(SDL_Rect){107,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'Q'){SDL_BlitSurface(font, &(SDL_Rect){114,0,7,11}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'R'){SDL_BlitSurface(font, &(SDL_Rect){122,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'S'){SDL_BlitSurface(font, &(SDL_Rect){130,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'T'){SDL_BlitSurface(font, &(SDL_Rect){137,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'U'){SDL_BlitSurface(font, &(SDL_Rect){144,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == 'V'){SDL_BlitSurface(font, &(SDL_Rect){152,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'W'){SDL_BlitSurface(font, &(SDL_Rect){159,0,10,9}, o, &dr); dr.x += 10+m;}
        if(s[i] == 'X'){SDL_BlitSurface(font, &(SDL_Rect){170,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'Y'){SDL_BlitSurface(font, &(SDL_Rect){177,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'Z'){SDL_BlitSurface(font, &(SDL_Rect){184,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'a'){SDL_BlitSurface(font, &(SDL_Rect){191,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'b'){SDL_BlitSurface(font, &(SDL_Rect){198,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'c'){SDL_BlitSurface(font, &(SDL_Rect){205,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 'd'){SDL_BlitSurface(font, &(SDL_Rect){211,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'e'){SDL_BlitSurface(font, &(SDL_Rect){218,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'f'){SDL_BlitSurface(font, &(SDL_Rect){225,0,4,9}, o, &dr); dr.x += 3+m;}
        if(s[i] == 'g'){SDL_BlitSurface(font, &(SDL_Rect){229,0,6,11}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'h'){SDL_BlitSurface(font, &(SDL_Rect){236,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'i'){SDL_BlitSurface(font, &(SDL_Rect){243,0,2,9}, o, &dr); dr.x += 2+m;}
        if(s[i] == 'j'){SDL_BlitSurface(font, &(SDL_Rect){246,0,3,11}, o, &dr); dr.x += 3+m;}
        if(s[i] == 'k'){SDL_BlitSurface(font, &(SDL_Rect){250,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'l'){SDL_BlitSurface(font, &(SDL_Rect){257,0,2,9}, o, &dr); dr.x += 2+m;}
        if(s[i] == 'm'){SDL_BlitSurface(font, &(SDL_Rect){260,0,10,9}, o, &dr); dr.x += 10+m;}
        if(s[i] == 'n'){SDL_BlitSurface(font, &(SDL_Rect){271,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'o'){SDL_BlitSurface(font, &(SDL_Rect){278,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'p'){SDL_BlitSurface(font, &(SDL_Rect){285,0,6,11}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'q'){SDL_BlitSurface(font, &(SDL_Rect){292,0,6,11}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'r'){SDL_BlitSurface(font, &(SDL_Rect){299,0,4,9}, o, &dr); dr.x += 4+m;}
        if(s[i] == 's'){SDL_BlitSurface(font, &(SDL_Rect){304,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == 't'){SDL_BlitSurface(font, &(SDL_Rect){310,0,4,9}, o, &dr); dr.x += 4+m;}
        if(s[i] == 'u'){SDL_BlitSurface(font, &(SDL_Rect){315,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'v'){SDL_BlitSurface(font, &(SDL_Rect){322,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'w'){SDL_BlitSurface(font, &(SDL_Rect){329,0,8,9}, o, &dr); dr.x += 8+m;}
        if(s[i] == 'x'){SDL_BlitSurface(font, &(SDL_Rect){338,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'y'){SDL_BlitSurface(font, &(SDL_Rect){345,0,6,11}, o, &dr); dr.x += 6+m;}
        if(s[i] == 'z'){SDL_BlitSurface(font, &(SDL_Rect){352,0,5,9}, o, &dr); dr.x += 5+m;}
        if(s[i] == '0'){SDL_BlitSurface(font, &(SDL_Rect){358,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '1'){SDL_BlitSurface(font, &(SDL_Rect){366,0,4,9}, o, &dr); dr.x += 4+m;}
        if(s[i] == '2'){SDL_BlitSurface(font, &(SDL_Rect){372,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '3'){SDL_BlitSurface(font, &(SDL_Rect){379,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '4'){SDL_BlitSurface(font, &(SDL_Rect){386,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '5'){SDL_BlitSurface(font, &(SDL_Rect){393,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '6'){SDL_BlitSurface(font, &(SDL_Rect){400,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '7'){SDL_BlitSurface(font, &(SDL_Rect){407,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '8'){SDL_BlitSurface(font, &(SDL_Rect){414,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == '9'){SDL_BlitSurface(font, &(SDL_Rect){421,0,6,9}, o, &dr); dr.x += 6+m;}
        if(s[i] == ':'){SDL_BlitSurface(font, &(SDL_Rect){428,0,2,9}, o, &dr); dr.x += 2+m;}
        if(s[i] == '.'){SDL_BlitSurface(font, &(SDL_Rect){431,0,2,9}, o, &dr); dr.x += 2+m;}
        if(s[i] == '+'){SDL_BlitSurface(font, &(SDL_Rect){435,0,7,9}, o, &dr); dr.x += 7+m;}
        if(s[i] == '-'){SDL_BlitSurface(font, &(SDL_Rect){443,0,4,9}, o, &dr); dr.x += 4+m;}
        if(s[i] == ' '){dr.x += 2+m;}
    }
}

#endif
