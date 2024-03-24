#include "fixed_point.h"

/**
 * fixed_power_int - compute: x^n, in O(log n) time
 *
 * @x:         base of the power
 * @frac_bits: fractional bits of @x
 * @n:         power to raise @x to.
 *
 * By exploiting the relation between the definition of the natural power
 * function: x^n := x*x*...*x (x multiplied by itself for n times), and
 * the binary encoding of numbers used by computers: n := \Sum n_i * 2^i,
 * (where: n_i \elem {0, 1}, the binary vector representing n),
 * we find: x^n := x^(\Sum n_i * 2^i) := \Prod x^(n_i * 2^i), which is
 * of course trivially computable in O(log_2 n), the length of our binary
 * vector.
 */
static unsigned long fixed_power_int(unsigned long x,
                                     unsigned int frac_bits,
                                     unsigned int n)
{
    unsigned long result = 1UL << frac_bits;

    if (n) {
        for (;;) {
            if (n & 1) {
                result *= x;
                result += 1UL << (frac_bits - 1); /* round up unconditional */
                result >>= frac_bits;
            }
            n >>= 1;
            if (!n)
                break;
            x *= x;
            x += 1UL << (frac_bits - 1);
            x >>= frac_bits;
        }
    }

    return result;
}

static void print_fixed(fixed_point_t x)
{
    printf("%ld.%02ld\n", LOAD_INT(x), LOAD_FRAC(x));
}

/**
 * return a * b in fixed-point format
 */
fixed_point_t multi_f(fixed_point_t a, fixed_point_t b)
{
    unsigned long result = a * b;
    result += 1UL << (FSHIFT - 1);
    result >>= FSHIFT;

    return result;
}
/**
 * return a / b in fixed-point
 * take scale factor so fractions of the result can be kept
 */
fixed_point_t div_f(fixed_point_t a, fixed_point_t b)
{
    unsigned long result = (a << SCALE_FACTOR) / b;
    result = LOAD_FIXED(result);
    result >>= SCALE_FACTOR;

    return result;
}


/**
 * using divison-free approximation of sqrt
 * ref: https://hackmd.io/@vax-r/linux2024-homework1#Monte-Carlo-Search-Tree
 */
fixed_point_t sqrt_f(fixed_point_t num)
{
    fixed_point_t res = 0L;
    /* i is intialized as the index of first bit 1 in num */
    /* t starts from num, divided by two each time */
    for (int i = 31 - __builtin_clz(num | 1); i >= 0; i--) {
        fixed_point_t t = 1UL << i;
        if (multi_f(res + t, res + t) <= num)
            res += t;
    }
    return res;
}

/**
 * using recursion to find log2 first
 * ref: https://github.com/dmoulding/log2fix/blob/master/log2fix.c
 */
fixed_point_t log2_f(fixed_point_t num)
{
    if (num <= 0)
        return INT32_MIN;

    /* b = 1/2 */
    fixed_point_t b = LOAD_FIXED(1L) >> 1;
    fixed_point_t y = 0;

    /* make num in [1, 2) */
    while (num < LOAD_FIXED(1L)) {
        num <<= 1;
        y -= LOAD_FIXED(1L);
    }
    while (num >= LOAD_FIXED(2L)) {
        num >>= 1;
        y += LOAD_FIXED(1L);
    }

    fixed_point_t z = num;
    for (size_t i = 0; i < FSHIFT; i++) {
        z = multi_f(z, z);
        /* if x >= 2: x = x/2, y = y+b */
        if (z >= LOAD_FIXED(2L)) {
            z >>= 1;
            y += b;
        }
        b >>= 1;
    }

    return y;
}

fixed_point_t loge_f(fixed_point_t num)
{
    uint64_t t = log2_f(num) * INV_LOG2_E;
    return t >> 31;
}

int main(void)
{
    fixed_point_t x = LOAD_FIXED(0b01010010);
    fixed_point_t y = LOAD_FIXED(0b011010010);
    print_fixed(x);
    print_fixed(y);
    y = multi_f(x, y);
    printf("x / y = %ld.%ld\n", LOAD_INT(y), LOAD_FRAC(y));
    print_fixed(sqrt_f(LOAD_FIXED(512L)));
    print_fixed(loge_f(LOAD_FIXED(512L)));
}