#ifndef LAB0_FIXEDPOINT_H
#define LAB0_FIXEDPOINT_H

#include <stdio.h>

/* Linux-kernel fixed point arthimetic implementation */

#define FSHIFT 11             /* nr of bits of precision */
#define FIXED_1 (1 << FSHIFT) /* 1.0 as fixed point */

#define LOAD_FIXED(x) ((x) << FSHIFT) /* change int to fixed-point */
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) \
    LOAD_INT(((x) & (FIXED_1 - 1)) * 100) /* keep 2 digits of fractions */
#define SCALE_FACTOR 3 /* this is used to keep the fractions of division */
typedef unsigned long fixed_point_t;

#endif