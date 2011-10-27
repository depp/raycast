#ifndef DEFS_H
#define DEFS_H

/* Define BIG_ENDIAN, LITTLE_ENDIAN, and BYTE_ORDER.  Tries to work on
   as many platforms as possible.  Usese the definitions from a
   system-provided header file when it is available.  Otherwise, uses
   built-in preprocessor macros if available.  As a last resort,
   examines machine type macros.

   You should NOT check byte order in a configuration script.  Mac OS
   X supports simultaneous compilation for architectures with
   different byte orders.  */
#if defined(__linux__)
#include <endian.h>
#elif defined(__APPLE__)
#include <machine/endian.h>
#else

#define BIG_ENDIAN 4321
#define LITTLE_ENDIAN 1234

#if defined(__BIG_ENDIAN__) && __BIG_ENDIAN__
#define BYTE_ORDER BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__) && __LITTLE_ENDIAN__
#define BYTE_ORDER LITTLE_ENDIAN
/* This line is taken from LibSDL's SDL_endian.h header file.  */
#elif defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif

#endif
