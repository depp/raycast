#include "draw.h"
#include "imath.h"
#include "texture.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void texture_make(struct texture **tex, unsigned char *ptr,
                  unsigned w, unsigned h, unsigned rb, unsigned flags)
{
    struct texture *tp;
    unsigned wb, hb, count, mcount, i, coff, off[11], msz, tsz;

    if (flags & TEXTURE_COLUMN) {
        wb = ilog2(h);
        hb = ilog2(w);
    } else {
        wb = ilog2(w);
        hb = ilog2(h);
    }
    count = (wb > hb ? wb : hb) + 1;
    mcount = (wb > hb ? hb : wb) + 1;
    assert(count <= 11);
    tp = malloc(sizeof(*tp) + sizeof(*tp->pixels) * count);
    if (!tp)
        abort();
    tp->wbits = wb;
    tp->hbits = hb;
    tp->count = count;

    coff = 0;
    msz = 1u << (count + mcount);
    for (i = 0; i + 1 < mcount; ++i) {
        off[i] = coff;
        coff += msz;
        msz >>= 2;
    }
    for (; i < count; ++i) {
        off[i] = coff;
        coff += msz;
        msz >>= 1;
    }
    tsz = coff;

    if (flags & TEXTURE_COLOR) {
        unsigned *tptr;
        unsigned x, y, p;
        tptr = malloc(tsz * 4);
        if (!tptr)
            abort();
        for (i = 0; i < count; ++i)
            tp->pixels[i] = tptr + off[i];

        if (flags & TEXTURE_COLUMN) {
            for (y = 0; y < h; ++y) {
                for (x = 0; x < w; ++x) {
                    p = ((unsigned) ptr[y*rb+x*3+0] << RSHIFT) |
                        ((unsigned) ptr[y*rb+x*3+1] << GSHIFT) |
                        ((unsigned) ptr[y*rb+x*3+2] << BSHIFT);
                    tptr[y+x*(1u<<hb)] = p;
                }
            }

            /* Zero remaining space */
        } else {
            for (y = 0; y < h; ++y) {
                for (x = 0; x < w; ++x) {
                    p = ((unsigned) ptr[y*rb+x*3+0] << RSHIFT) |
                        ((unsigned) ptr[y*rb+x*3+1] << GSHIFT) |
                        ((unsigned) ptr[y*rb+x*3+2] << BSHIFT);
                    tptr[x+y*(1u<<wb)] = p;
                }
            }

            /* Zero remaining space */
        }

        /* Calculate mip maps */
    } else {
        unsigned char *tptr;
        unsigned x, y, p;
        tptr = malloc(tsz);
        if (!tptr)
            abort();
        for (i = 0; i < count; ++i)
            tp->pixels[i] = tptr + off[i];

        if (flags & TEXTURE_COLUMN) {
            for (y = 0; y < h; ++y)
                for (x = 0; x < w; ++x)
                    tptr[y+x*(1u<<hb)] = ptr[y*rb+x];

            /* Zero remaining space */
        } else {
            if ((1u << wb) == w) {
                memcpy(tptr, ptr, w * h);
            } else {
                for (y = 0; y < h; ++i)
                    memcpy(tptr + y * (1u<<wb), ptr + y * w, w);
            }

            /* Zero remaining space */
        }

        /* Calculate mip maps */
    }

    *tex = tp;
}
