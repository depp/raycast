#include "SDL.h"
#include "defs.h"
#include "draw.h"
#include "input.h"
#include "level.h"
#include "texture.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct texture *g_textures[3];

#define TIME 5
/* Units / tick */
#define TURN 4000
/* Units / tick */
#define FORWARD 300
#define BACKWARD 300

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

__attribute__((malloc))
void *xmalloc(size_t sz)
{
    void *p;
    if (!sz)
        return NULL;
    p = malloc(sz);
    if (!p)
        fail("out of memory");
    return p;
}

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
    SDL_Event e;
    SDL_Joystick *joy;
    unsigned reftime, lasttime, curtime, delta, i, keys = 0, kval;
    int turn, turn2 = 0, speed, speed2 = 0, strafe2 = 0, njoy, v;
    struct in_axis angle;

/*
    double t = 0, lt, dt;
    unsigned reftime;
    int kval;

    double perftime;
    unsigned framecount;
    clock_t c1, c2;

    float px = 0, py = 0, pa = 0;
    unsigned keys = 0;
*/

    struct pixbuf buf;

    struct world *w;
    struct obj *p;

    (void) argc;
    (void) argv;

    texture_load(&g_textures[0], "brick.jpg");
    texture_load(&g_textures[1], "roughstone.jpg");
    texture_load(&g_textures[2], "ivy.jpg");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK))
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

    njoy = SDL_NumJoysticks();
    if (!njoy)
        joy = NULL;
    else
        joy = SDL_JoystickOpen(0);

    w = world_new();
    p = &w->player;
    in_axis_init(&angle);
    lasttime = reftime = SDL_GetTicks();
    // perftime = 0;
    // framecount = 0;
    // c1 = clock();
    while (1) {
        curtime = SDL_GetTicks();
        delta = curtime - reftime;
        if (delta > 1000) {
            puts("Lag");
            delta = lasttime - reftime;
            reftime = curtime - delta;
        }

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

            case SDL_JOYAXISMOTION:
                v = -e.jaxis.value;
                switch (e.jaxis.axis) {
                case 0: strafe2 = v * FORWARD >> 15; break; /* left x */
                case 1: speed2  = v * FORWARD >> 15; break; /* left y */
                case 2: turn2   = v * TURN    >> 15; break; /* right x */
                case 3: break; /* right y */
                }
                break;
            }
        }

        turn = 0;
        if (keys & (1u << KLEFT)) {
            if ((keys & (1u << KRIGHT)) == 0)
                turn = TURN;
        } else if (keys & (1u << KRIGHT)) {
            turn = -TURN;
        }
        in_axis_setvel(&angle, delta, turn + turn2);

        if (delta >= 64) {
            speed = 0;
            if (keys & (1u << KUP)) {
                if ((keys & (1u << KDOWN)) == 0)
                    speed = FORWARD;
            } else if (keys & (1u << KDOWN)) {
                speed = - BACKWARD;
            }

            p->speed = speed + speed2;
            p->strafe = strafe2;
            for (i = 0; 64 * (i + 1) <= delta; ++i) {
                p->angle = in_axis_get(&angle, i * 64) >> 6;
                world_update(w);
            }
            in_axis_advance(&angle, 64 * i);
            delta &= 63;
            reftime = curtime - delta;
        }

        /*
        if (t > perftime + 1.0) {
            c2 = clock();
            printf("%.1f fps, %.1f%% processor\n",
                   framecount / (t - perftime),
                   100 * (c2 - c1) / ((t - perftime) * CLOCKS_PER_SEC));
            framecount = 0;
            c1 = c2;
            perftime = t;
        }
        framecount++;
        */

        SDL_LockSurface(vid);
        buf.ptr = vid->pixels;
        buf.width = vid->w;
        buf.height = vid->h;
        buf.row = vid->pitch / 4;

        memset(buf.ptr, 0, buf.height * buf.row * 4);

        {
            int d = delta;
            int x = p->x0 + ((p->x1 - p->x0) * d >> 6);
            int y = p->y0 + ((p->y1 - p->y0) * d >> 6);
            p->angle = in_axis_get(&angle, delta);
            level_draw(&buf, x, y, p->angle >> 6);
        }

/*
        draw_rect(&buf, 10, 20,
                  (buf.width - 20) * (fmod(t, TIME) * (1.0/TIME)), 5,
                  rgb(255, 32, 32));
*/

        SDL_UpdateRect(vid, 0, 0, 0, 0);
        SDL_UnlockSurface(vid);
    }
quit:

    SDL_Quit();
    return 0;
}
