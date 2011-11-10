#include "defs.h"
#include "imath.h"
#include "world.h"
#include <stdlib.h>
// #include <stdio.h>

struct world *world_new(void)
{
    struct world *w = xmalloc(sizeof(*w));
    struct obj *o = &w->player;
    o->x0 = 0;
    o->y0 = 0;
    o->x1 = 0;
    o->y1 = 0;
    o->angle = 0;
    o->speed = 0;
    o->strafe = 0;
    return w;
}

void world_delete(struct world *w)
{
    free(w);
}

void world_update(struct world *w)
{
    int ax, ay, dx, dy;
    struct obj *o = &w->player;

    o->x0 = o->x1;
    o->y0 = o->y1;
    ax = icos(o->angle);
    ay = isin(o->angle);
    dx = ax * o->speed - ay * o->strafe;
    dy = ay * o->speed + ax * o->strafe;
    o->x1 += dx / (1 << 14);
    o->y1 += dy / (1 << 14);
    // printf("dx %d dy %d\n", dx / (1 << 14), dy / (1 << 14));
}
