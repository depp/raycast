#include "imath.h"
#include "defs.h"

static const unsigned short SIN_TABLE[65] = {
    0, 804, 1608, 2411, 3212, 4011, 4808, 5602,
    6393, 7180, 7962, 8740, 9513, 10279, 11040, 11794,
    12540, 13280, 14011, 14734, 15448, 16152, 16847, 17532,
    18206, 18869, 19521, 20161, 20789, 21404, 22007, 22596,
    23172, 23733, 24281, 24813, 25331, 25834, 26321, 26792,
    27247, 27686, 28107, 28512, 28900, 29271, 29623, 29958,
    30275, 30574, 30854, 31116, 31359, 31583, 31788, 31973,
    32140, 32287, 32415, 32523, 32612, 32681, 32730, 32760,
    32768
};

int isin(unsigned x)
{
    unsigned ABITS = 16, QBITS = 2, TBITS = 6,
        FBITS = ABITS - QBITS - TBITS,
        Q = 1 << TBITS, F = 1 << FBITS;
    unsigned q = (x >> (ABITS - QBITS)) & 3,
        i = (x >> (ABITS - QBITS - TBITS)) & (Q - 1), j = x & (F - 1);
    int a1, a2;

    switch (q) {
    case 0:
        a1 = SIN_TABLE[i];
        a2 = SIN_TABLE[i+1];
        break;

    case 1:
        a1 = SIN_TABLE[Q - i];
        a2 = SIN_TABLE[Q - 1 - i];
        break;

    case 2:
        a1 = -SIN_TABLE[i];
        a2 = -SIN_TABLE[i + 1];
        break;

    case 3:
        a1 = -SIN_TABLE[Q - i];
        a2 = -SIN_TABLE[Q - 1 - i];
        break;
    }

    return (int) (a1 * (F - j) + a2 * j + F) >> (FBITS + 1);
}

int icos(unsigned x)
{
    return isin(x + 0x4000);
}

unsigned ilog2(unsigned x)
{
    unsigned i;
    for (i = 0; i < 32 && (1U << i) < x; ++i);
    return i;
}

#if defined(ISIN_TEST)
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int i;
    double maxd = 0, err = 0, x, d;
    for (i = 0; i < (1 << 16); ++i) {
        x = sin(i * (atan(1) * 8 / 65536.0));
        d = fabs(x - isin(i) / 16384.0);
        if (d > maxd)
            maxd = d;
        err += d * d;
    }
    printf(
        "error max: %f\n"
        "error dev: %f\n",
        maxd,
        sqrt(err / 65536.0));
    return 0;
}

#endif
