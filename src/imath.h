#ifndef IMATH_H
#define IMATH_H

/* Trig functions take angles such that 0x10000 is a full circle, so
   any angle can be stored in 16 bits.  They return numbers in the
   range -0x4000..+0x4000.  */
int isin(int x);
int icos(int x);

unsigned ilog2(unsigned x);

static inline unsigned align(unsigned sz)
{
    return (sz + 15) & ~15;
}

#endif
