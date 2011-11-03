#include "level.h"
#include "defs.h"
#include "imath.h"
#include "draw.h"
#include "texture.h"
#include <stdlib.h>
#include <assert.h>

struct rc_column {
    short dx, dy;
    short ax, ay;
};

/* Setup columns for perspective projection.  The number of columns is
   "w".  The camera points in the direction specified by "angle".  The
   value "a" is equal to 65536*tan(fov/2), e.g., 65536 for a 90 degree
   field of view.  */
static void perspective(struct rc_column *cols, unsigned w,
                        unsigned angle, int a)
{
    int dx, dy, dx2, dy2, b, s;
    unsigned i;
    dx = icos(angle);
    dy = isin(angle);
    /* We want dx2 such that, approximately,
       ((w/2) * dx2) >> s = dy * a >> 16
       So dx2 = dy * a / (w/2) >> (16 - s) */
    s = 16;
    dx2 = dy * a / (int)(w/2);
    dy2 = -dx * a / (int)(w/2);
    for (i = 0; i < w; ++i) {
        b = (int) i - w / 2;
        cols[i].dx = dx + ((b * dx2) >> s);
        cols[i].dy = dy + ((b * dy2) >> s);
        cols[i].ax = dx;
        cols[i].ay = dy;
    }
}

static const unsigned char LEVEL[8][8] = {
    { 1, 1, 3, 3, 1, 1, 1, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 4, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 4, 0, 0, 0, 0, 0, 0, 2 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 3, 3, 3, 3, 1, 3, 3 }
};

static void render(struct pixbuf *restrict buf,
                   int x, int y, struct rc_column *cols)
{
    int SBITS = 10, SWIDTH = 1 << SBITS;
    unsigned vw = buf->width, *vp = buf->ptr, vrb = buf->row,
        vh = buf->height, i;
    x <<= 4;
    y <<= 4;
    x += SWIDTH * 4;
    y += SWIDTH * 4;
    int cx0 = (x >> SBITS), cy0 = (y >> SBITS);
    int ox0 = x & (SWIDTH - 1), oy0 = y & (SWIDTH - 1);
    for (i = 0; i < vw; ++i) {
        unsigned *cp = vp + i;
        unsigned c; /* Color */

        if (cx0 < 1 || cy0 < 1 || cx0 >= 7 || cy0 >= 7) {
            c = rgb(64, 64, 32);
            goto solid;
        }

        /* To simplify the algorithm and avoid dividing by zero, we
           identify the "major axis" and "minor axis".  Each
           iteration, we move exactly one cell along the major axis
           and at most one cell along the minor axis.  The "slope" is
           the ratio of movement along the minor axis to the major
           axis, and is therefore its magnitude is at most 1.0
           (65536).  */

        /* dx: delta x, adx: absolute delta x, sdx: sign delta x
           cx: cell x, m: slope, off: minor axis offset,
           ox: offset on X axis */
        int dx, dy, adx, ady, sdx, sdy, cx, cy, m, off, ox, oy;
        dx = cols[i].dx;
        dy = cols[i].dy;
        adx = dx >= 0 ? dx : -dx;
        sdx = dx >= 0 ? 1 : -1;
        ady = dy >= 0 ? dy : -dy;
        sdy = dy >= 0 ? 1 : -1;
        cx = cx0;
        cy = cy0;
        if (adx >= ady) {
            m = (dy << 16) / adx;
            off = dx >= 0 ? SWIDTH - ox0 : ox0;
            off = oy0 + ((m * off) >> 16);
            while (1) {
                if (off >> SBITS) {
                    cy += sdy;
                    if (LEVEL[cx][cy]) {
                        if (dy >= 0)
                            off -= SWIDTH;
                        off = - off * dx / dy;
                        if (dx >= 0)
                            off += SWIDTH;
                        ox = off;
                        oy = dy >= 0 ? 0 : SWIDTH;
                        goto hit;
                    }
                    off &= SWIDTH - 1;
                }
                cx += sdx;
                if (LEVEL[cx][cy]) {
                    ox = dx >= 0 ? 0 : SWIDTH;
                    oy = off;
                    goto hit;
                }
                off += m >> (16 - SBITS);
            }
        } else {
            m = (dx << 16) / ady;
            off = dy >= 0 ? SWIDTH - oy0 : oy0;
            off = ox0 + ((m * off) >> 16);
            while (1) {
                if (off >> SBITS) {
                    cx += sdx;
                    if (LEVEL[cx][cy]) {
                        if (dx >= 0)
                            off -= SWIDTH;
                        off = - off * dy / dx;
                        if (dy >= 0)
                            off += SWIDTH;
                        oy = off;
                        ox = dx >= 0 ? 0 : SWIDTH;
                        goto hit;
                    }
                    off &= SWIDTH - 1;
                }
                cy += sdy;
                if (LEVEL[cx][cy]) {
                    oy = dy >= 0 ? 0 : SWIDTH;
                    ox = off;
                    goto hit;
                }
                off += m >> (16 - SBITS);
            }
        }
        c = rgb(32, 32, 32);
        goto solid;

    solid:
        {
            unsigned j;
            for (j = 0; j < vh; ++j)
                cp[vrb*j] = c;
        }
        continue;

    hit:
        {
            c = rgb(64, 64, 64);
            int hx = (cx << SBITS) + ox - x, hy = (cy << SBITS) + oy - y;
            int d = (cols[i].ax * hx + cols[i].ay * hy) >> 16;
            if (d < 10) {
                c = rgb(64, 32, 32);
                goto solid;
            }
            unsigned h = (240 * 256) / d;
            struct texture *t;

            switch (LEVEL[cx][cy]) {
            case 1: c = rgb(255, 32, 32); goto color;
            case 2: t = g_textures[0]; goto texture;
            case 3: t = g_textures[1]; goto texture;
            case 4: t = g_textures[2]; goto texture;
            }

        color:
            {
                if (h > 240)
                    h = 240;
                unsigned j;
                for (j = vh/2-h; j < vh/2+h; ++j)
                    cp[vrb*j] = c;
                continue;
            }

        texture:
            {
                unsigned hb = t->hbits, wb = t->wbits;
                unsigned tm = (1u << (hb + 16)) / (h * 2);
                unsigned n = 0, count = wb > hb ? hb : wb;
                while (tm > (2u << 15u) && n < count) {
                    tm >>= 1;
                    n += 1;
                }
                hb -= n;
                wb -= n;
                unsigned tx = ((ox + oy) >> (SBITS - wb)) & ((1u << wb) - 1);
                unsigned ty = 0;
                unsigned mask = (1 << hb) - 1;
                unsigned *tp = (unsigned *) t->pixels[n] + (tx << hb);
                unsigned j;
                if (h > 240) {
                    ty = tm * (h - 240);
                    h = 240;
                }
                for (j = vh/2-h; j < vh/2+h; ++j) {
                    cp[vrb*j] = tp[(ty >> 16) & mask];
                    ty += tm;
                }
                continue;
            }
        }
    }
}

void level_draw(struct pixbuf *restrict buf,
                int x, int y, unsigned angle)
{
    struct rc_column *cols;
    unsigned w = buf->width;
    cols = malloc(sizeof(*cols) * w);
    assert(cols);
    perspective(cols, w, angle, 65536);
    render(buf, x, y, cols);
    free(cols);
}

