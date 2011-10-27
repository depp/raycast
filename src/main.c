#include "SDL.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

#if BYTE_ORDER == BIG_ENDIAN
#define RSHIFT 8
#define GSHIFT 16
#define BSHIFT 24
#else
#define RSHIFT 16
#define GSHIFT 8
#define BSHIFT 0
#endif

__attribute__((noreturn))
static void sdlerr(const char *s)
{
    fprintf(stderr, "error: %s: %s\n", s, SDL_GetError());
    SDL_Quit();
    exit(1);
}

__attribute__((noreturn))
static void fail(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
    SDL_Quit();
    exit(1);
}

int main(int argc, char *argv[])
{
    SDL_Surface *vid;
    SDL_PixelFormat *f;

    (void) argc;
    (void) argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
        sdlerr("SDL_Init");

    vid = SDL_SetVideoMode(768, 480, 32, SDL_SWSURFACE);
    if (!vid)
        sdlerr("SDL_SetVideoMode");

    f = vid->format;
    if (f->BitsPerPixel != 32 || f->BytesPerPixel != 4 ||
        f->Rloss || f->Gloss || f->Bloss ||
        f->Rshift != RSHIFT ||
        f->Gshift != GSHIFT ||
        f->Bshift != BSHIFT ||
        f->Rmask != (0xffu << RSHIFT) ||
        f->Gmask != (0xffu << GSHIFT) ||
        f->Bmask != (0xffu << BSHIFT))
    {
        printf("Byte order: %d\n", BYTE_ORDER);
        fprintf(
            stderr,
            "pixel format: %08x %08x %08x\n",
            f->Rmask, f->Gmask, f->Bmask);
        fail("unsupported pixel format");
    }

    SDL_Quit();
    return 0;
}
