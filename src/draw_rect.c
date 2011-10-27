#include "defs.h"
#include "draw.h"

void draw_rect(int x, int y, int w, int h, unsigned color)
{
    unsigned char *vp = video_ptr;
    int vrb = video_rowbytes, vw = video_width, vh = video_height;
    int x0 = x, x1 = x0 + w, y1 = vh - 1 - y, y0 = y1 - h, xi, yi;
    if (y0 < 0) y0 = 0;
    if (y1 > vh) y1 = vh;
    if (x0 < 0) x0 = 0;
    if (x1 > vw) x1 = vw;
    for (yi = y0; yi < y1; ++yi)
        for (xi = x0; xi < x1; ++xi)
            ((unsigned *) (vp + vrb * yi))[xi] = color;
}
