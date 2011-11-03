#include "defs.h"
#include "draw.h"

void draw_rect(struct pixbuf *restrict buf,
               int x, int y, int w, int h, unsigned color)
{
    unsigned *vp = buf->ptr;
    unsigned vr = buf->row, vw = buf->width, vh = buf->height;
    int x0 = x, x1 = x0 + w, y1 = vh - 1 - y, y0 = y1 - h, xi, yi;
    if (y0 < 0) y0 = 0;
    if (y1 > (int) vh) y1 = vh;
    if (x0 < 0) x0 = 0;
    if (x1 > (int) vw) x1 = vw;
    for (yi = y0; yi < y1; ++yi)
        for (xi = x0; xi < x1; ++xi)
            vp[vr * yi + xi] = color;
}
