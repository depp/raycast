#include "SDL.h"
#include "defs.h"
#include "draw.h"
#include <stdio.h>
#include <stdlib.h>

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
    double t;
    unsigned reftime;

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

    reftime = SDL_GetTicks();
    while (1) {
        t = 0.001 * (SDL_GetTicks() - reftime);
        if (t > 5)
            break;
        SDL_LockSurface(vid);
        video_ptr = vid->pixels;
        video_width = vid->w;
        video_height = vid->h;
        video_rowbytes = vid->pitch;
        memset(video_ptr, 0, video_height * video_rowbytes);
        draw_rect(10, 20, (video_width - 20) * (t * (1/5.0)), 5,
                  rgb(255, 32, 32));
        SDL_UpdateRect(vid, 0, 0, 0, 0);
        SDL_UnlockSurface(vid);
    }

    SDL_Quit();
    return 0;
}
