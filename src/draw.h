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

struct pixbuf {
    unsigned *ptr;
    unsigned width, height, row;
};

static inline unsigned rgb(unsigned r, unsigned g, unsigned b)
{
    return (r << RSHIFT) | (g << GSHIFT) | (b << BSHIFT);
}

void draw_rect(struct pixbuf *restrict buf, int x, int y, int w, int h,
               unsigned color);

#endif
