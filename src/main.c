#include "SDL.h"
#include "defs.h"
#include "draw.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME 5
/* Radians / sec */
#define TURN 6
/* Units / sec */
#define FORWARD 150
#define BACKWARD 150

#define SIZE 32

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

struct dot {
    int x, y;
    unsigned c;
};

enum {
    KLEFT,
    KRIGHT,
    KUP,
    KDOWN
};

int main(int argc, char *argv[])
{
    SDL_Surface *vid;
    SDL_PixelFormat *f;
    double t = 0, lt, dt;
    unsigned reftime;
    SDL_Event e;
    int kval, i;

    struct dot dots[4];
    float px = 0, py = 0, pa = 0;
    unsigned keys = 0;

    dots[0].x = -256;
    dots[0].y = -256;
    dots[0].c = rgb(255, 32, 32);
    dots[1].x = 256;
    dots[1].y = -256;
    dots[1].c = rgb(220, 220, 0);
    dots[2].x = -256;
    dots[2].y = 256;
    dots[2].c = rgb(64, 40, 255);
    dots[3].x = 256;
    dots[3].y = 256;
    dots[3].c = rgb(10, 200, 30);

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
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                goto quit;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: goto quit;
                case SDLK_UP: kval = KUP; break;
                case SDLK_DOWN: kval = KDOWN; break;
                case SDLK_LEFT: kval = KLEFT; break;
                case SDLK_RIGHT: kval = KRIGHT; break;
                default: goto nokey;
                }
                if (e.type == SDL_KEYDOWN)
                    keys |= 1u << kval;
                else
                    keys &= ~(1u << kval);
            nokey:
                break;
            }
        }

        lt = t;
        t = 0.001 * (SDL_GetTicks() - reftime);
        dt = t - lt;

        if (keys & (1u << KLEFT)) {
            if ((keys & (1u << KRIGHT)) == 0) {
                pa += TURN * dt;
                if (pa > 8 * atanf(1))
                    pa -= 8 * atanf(1);
            }
        } else if (keys & (1u << KRIGHT)) {
            pa -= TURN * dt;
            if (pa < 0)
                pa = 8 * atanf(1);
        }
        if (keys & (1u << KUP)) {
            if ((keys & (1u << KDOWN)) == 0) {
                px += cosf(pa) * (FORWARD * dt);
                py += sinf(pa) * (FORWARD * dt);
            }
        } else if (keys & (1u << KDOWN)) {
            px -= cosf(pa) * (BACKWARD * dt);
            py -= sinf(pa) * (BACKWARD * dt);
        }

        SDL_LockSurface(vid);
        video_ptr = vid->pixels;
        video_width = vid->w;
        video_height = vid->h;
        video_rowbytes = vid->pitch;

        memset(video_ptr, 0, video_height * video_rowbytes);

        float dx = cosf(pa), dy = sinf(pa);
        for (i = 0; i < 4; ++i) {
            float ix = dots[i].x - px, iy = dots[i].y - py;
            float w = ix * dx + iy * dy;
            if (w < 1)
                continue;
            float u = ix * dy - iy * dx;
            int sz = SIZE * (video_width / (2*w));
            int x = u * (video_width / (2*w)) + video_width / 2;
            if (sz < 2)
                sz = 2;
            draw_rect(x - sz/2, video_height/2 -sz*2, sz, sz, dots[i].c);
            draw_rect(x - sz/2, video_height/2 +sz, sz, sz, dots[i].c);
        }

        draw_rect(10, 20, (video_width - 20) * (fmod(t, TIME) * (1.0/TIME)), 5,
                  rgb(255, 32, 32));

        SDL_UpdateRect(vid, 0, 0, 0, 0);
        SDL_UnlockSurface(vid);
    }
quit:

    SDL_Quit();
    return 0;
}
