#include "level.h"
#include "defs.h"
#include "imath.h"
#include "draw.h"
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
    { 1, 1, 1, 1, 1, 1, 1, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 0, 0, 0, 0, 0, 0, 4 },
    { 2, 3, 3, 3, 3, 3, 3, 3 }
};

static void render(int x, int y, struct rc_column *cols)
{
    unsigned SBITS = 10, SWIDTH = 1 << SBITS,
        w = video_width, *vp = video_ptr, vrb = video_rowbytes / 4,
        vh = video_height, i;
    x <<= 4;
    y <<= 4;
    x += SWIDTH * 4;
    y += SWIDTH * 4;
    int cx0 = (x >> SBITS), cy0 = (y >> SBITS);
    unsigned ox0 = x & (SWIDTH - 1), oy0 = y & (SWIDTH - 1);
    for (i = 0; i < w; ++i) {
        unsigned *cp = vp + i;
        int dx = cols[i].dx, dy = cols[i].dy;
        int cx = cx0, cy = cy0;
        unsigned udx, udy; /* Unsigned dx, dy */
        unsigned m; /* Slope */
        unsigned off, loff; /* Offset in cell on minor axis */
        int ox, oy; /* Final offset within cell */
        unsigned c; /* Color */
        if (cx < 0 || cy < 0 || cx >= 8 || cy >= 8)
            goto oob;
        if (dx >= 0) {
            if (dy >= 0) {
                udx = dx, udy = dy;
                if (udx >= udy) {
                    m = (udy << 16) / udx;
                    loff = oy0;
                    off = loff + ((m * (SWIDTH - ox0)) >> 16);
                    if (off >= SWIDTH) {
                        off &= SWIDTH - 1;
                        cy += 1;
                        // if (cy == 8) goto miss;
                        if (0 && LEVEL[cx][cy]) {
                            oy = 0;
                            ox = ox0 + (SWIDTH - off) * udx / udy;
                            goto hit;
                        }
                    }
                    while (1) {
                        cx += 1;
                        // if (cx == 8) goto miss;
                        if (cx == 7) {
                            // LEVEL[cx][cy]
                            ox = 0;
                            oy = off;
                            goto hit;
                        }
                        loff = off;
                        off = loff + (m >> (16 - SBITS));
                        if (off >= SWIDTH) {
                            off &= SWIDTH - 1;
                            cy += 1;
                            // if (cy == 8) goto miss;
                            if (0 && LEVEL[cx][cy]) {
                                oy = 0;
                                ox = (SWIDTH - off) * udx / udy;
                                goto hit;
                            }
                        }
                    }
                } else {
                    
                }
            } else {
                
            }
        } else {
            if (dy >= 0) {
                
            } else {
                
            }
        }
        c = rgb(32, 32, 32);
        goto solid;

    oob:
        c = rgb(64, 64, 32);
        goto solid;

    solid:
        {
            unsigned j;
            for (j = 0; j < vh; ++j)
                cp[vrb*j] = c;
        }
        continue;

    miss:
        c = rgb(64, 32, 32);
        goto solid;

    hit:
        {
            unsigned c = rgb(64, 64, 64);
            int hx = (cx << SBITS) + ox - x, hy = (cy << SBITS) + oy - y;
            int d = (cols[i].ax * hx + cols[i].ay * hy) >> 16;
            if (d < 10)
                goto miss;
            unsigned h = (240 * 256) / d;
            if (h > 240)
                h = 240;
            unsigned j;
            for (j = vh/2-h; j < vh/2+h; ++j)
                cp[vrb*j] = c;
        }
        continue;
    }
}

void level_draw(int x, int y, unsigned angle)
{
    struct rc_column *cols;
    unsigned w = video_width;
    cols = malloc(sizeof(*cols) * w);
    assert(cols);
    perspective(cols, w, angle, 65536);
    render(x, y, cols);
    free(cols);
}

