#include "imath.h"
#include "defs.h"

static const unsigned short SIN_TABLE[129] = {
    0, 402, 804, 1206, 1608, 2009, 2411, 2811,
    3212, 3612, 4011, 4410, 4808, 5205, 5602, 5998,
    6393, 6787, 7180, 7571, 7962, 8351, 8740, 9127,
    9512, 9896, 10279, 10660, 11039, 11417, 11793, 12167,
    12540, 12910, 13279, 13646, 14010, 14373, 14733, 15091,
    15447, 15800, 16151, 16500, 16846, 17190, 17531, 17869,
    18205, 18538, 18868, 19195, 19520, 19841, 20160, 20475,
    20788, 21097, 21403, 21706, 22006, 22302, 22595, 22884,
    23170, 23453, 23732, 24008, 24279, 24548, 24812, 25073,
    25330, 25583, 25833, 26078, 26320, 26557, 26791, 27020,
    27246, 27467, 27684, 27897, 28106, 28311, 28511, 28707,
    28899, 29086, 29269, 29448, 29622, 29792, 29957, 30118,
    30274, 30425, 30572, 30715, 30853, 30986, 31114, 31238,
    31357, 31471, 31581, 31686, 31786, 31881, 31972, 32058,
    32138, 32214, 32286, 32352, 32413, 32470, 32522, 32568,
    32610, 32647, 32679, 32706, 32729, 32746, 32758, 32766,
    32768
};

int isin(unsigned x)
{
    unsigned ABITS = 16, QBITS = 2, TBITS = 7,
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
