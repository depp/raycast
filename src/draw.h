#ifndef DRAW_H
#define DRAW_H
/* The only acceptable video format (for now, at least) is the BGRA
   pixel format, 8 bits per sample.  */
#include "defs.h"

#if BYTE_ORDER == BIG_ENDIAN
enum {
    RSHIFT = 8,
    GSHIFT = 16,
    BSHIFT = 24
};
#elif BYTE_ORDER == LITTLE_ENDIAN
enum {
    RSHIFT = 16,
    GSHIFT = 8,
    BSHIFT = 0
};
#else
#error No byte order
#endif

extern void *video_ptr;
extern unsigned video_width, video_height, video_rowbytes;

static inline unsigned rgb(unsigned r, unsigned g, unsigned b)
{
    return (r << RSHIFT) | (g << GSHIFT) | (b << BSHIFT);
}

void draw_rect(int x, int y, int w, int h, unsigned color);

#endif
