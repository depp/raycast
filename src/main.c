#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

__attribute__((noreturn))
static void sdlerr(const char *s)
{
    fprintf(stderr, "error: %s: %s\n", s, SDL_GetError());
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
    printf(
        "bits pp: %d\n"
        "bytes pp: %d\n"
        "loss: %d %d %d %d\n"
        "shift: %d %d %d %d\n"
        "mask: %08x %08x %08x %08x\n",
        f->BitsPerPixel,
        f->BytesPerPixel,
        f->Rloss, f->Gloss, f->Bloss, f->Aloss,
        f->Rshift, f->Gshift, f->Bshift, f->Ashift,
        f->Rmask, f->Gmask, f->Bmask, f->Amask);

    SDL_Quit();
    return 0;
}
