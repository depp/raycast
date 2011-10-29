#ifndef IMATH_H
#define IMATH_H

/* Trig functions take angles such that 0x10000 is a full circle, so
   any angle can be stored in 16 bits.  They return numbers in the
   range -0x4000..+0x4000.  */
int isin(unsigned x);
int icos(unsigned x);

#endif