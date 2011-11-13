#include "input.h"
#include <stdlib.h>

void in_axis_init(struct in_axis *a)
{
    a->points = NULL;
    a->pcount = 0;
    a->palloc = 0;
}

void in_axis_destroy(struct in_axis *a)
{
    free(a->points);
}

int in_axis_get(struct in_axis *a, int time)
{
    struct in_point *p = a->points;
    unsigned i, n = a->pcount;
    for (i = 0; i < n && p[i].time <= time; ++i);
    if (i > 0)
        return p[i-1].pos + p[i-1].vel * (time - p[i-1].time);
    else if (n > 0)
        return p[0].pos;
    else
        return 0;
}

static struct in_point *in_axis_getnew(struct in_axis *a, int time)
{
    struct in_point *p = a->points;
    unsigned i, n = a->pcount, nalloc;
    int pos, vel;
    for (i = 0; i < n && p[i].time <= time; ++i);
    if (i > 0) {
        pos = p[i-1].pos + p[i-1].vel * (time - p[i-1].time);
        vel = p[i-1].vel;
    } else {
        pos = 0;
        vel = 0;
    }
    if (i >= a->palloc) {
        nalloc = a->palloc ? a->palloc * 2 : 8;
        p = realloc(p, sizeof(*p) * nalloc);
        if (!p)
            abort();
        a->points = p;
        a->palloc = nalloc;
    }
    a->pcount = i + 1;
    p[i].time = time;
    p[i].pos = pos;
    p[i].vel = vel;
    return &p[i];
}

void in_axis_setpos(struct in_axis *a, int time, int pos)
{
    struct in_point *p = in_axis_getnew(a, time);
    p->pos = pos;
}

void in_axis_setvel(struct in_axis *a, int time, int vel)
{
    struct in_point *p;
    if (a->pcount) {
        p = &a->points[a->pcount - 1];
        if (p->time <= time && p->vel == vel)
            return;
    } else if (!vel) {
        return;
    }
    p = in_axis_getnew(a, time);
    p->vel = vel;
}

void in_axis_advance(struct in_axis *a, int time)
{
    struct in_point *p = a->points;
    unsigned i, j, n = a->pcount;
    for (i = 0; i < n && p[i].time <= time; ++i);
    if (i > 0)
        i--;
    if (i > 0) {
        for (j = 0; j < n - i; ++j) {
            p[j].time = p[j+i].time - time;
            p[j].pos  = p[j+i].pos;
            p[j].vel  = p[j+i].vel;
        }
        n = n - i;
    } else {
        for (j = 0; j < n; ++j)
            p[j].time -= time;
    }
    if (n > 0 && p[0].time < 0) {
        p[0].pos -= p[0].vel * p[0].time;
        p[0].time = 0;
    }
    a->pcount = n;
}
