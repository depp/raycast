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
        unsigned *tptr, *tp2;
        unsigned x, y, p, q0, q1, q2, q3, c1, c2;
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

        for (i = 1; i < mcount; ++i) {
            tptr = tp->pixels[i-1];
            tp2 = tp->pixels[i];
            for (y = 0; y < (1u << (hb - i)); ++y) {
                for (x = 0; x < (1u << (wb - i)); ++x) {
                    q0 = tptr[(2*y+0)*(1u<<(wb+1-i))+2*x+0];
                    q1 = tptr[(2*y+0)*(1u<<(wb+1-i))+2*x+1];
                    q2 = tptr[(2*y+1)*(1u<<(wb+1-i))+2*x+0];
                    q3 = tptr[(2*y+1)*(1u<<(wb+1-i))+2*x+1];
                    c1 = 0x00800080 +
                        ((q0 & 0xff00ff00) >> 2) +
                        ((q1 & 0xff00ff00) >> 2) +
                        ((q2 & 0xff00ff00) >> 2) +
                        ((q3 & 0xff00ff00) >> 2);
                    c2 = 0x00020002 +
                        (q0 & 0x00ff00ff) +
                        (q1 & 0x00ff00ff) +
                        (q2 & 0x00ff00ff) +
                        (q3 & 0x00ff00ff);
                    tp2[y*(1u<<(wb-i))+x] =
                        (c1 & 0xff00ff00) | ((c2 >> 2) & 0x00ff00ff);
                }
            }
        }
    } else {
        unsigned char *tptr;
        unsigned x, y;
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
