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
static fixed_point_t fixed_power_int(fixed_point_t x,
                                     unsigned int frac_bits,
                                     unsigned int n)
{
    fixed_point_t result = 1UL << frac_bits;

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
    printf("%lu.%02lu\n", LOAD_INT(x), LOAD_FRAC(x));
}

/**
 * return a * b in fixed-point format
 */
fixed_point_t multi(fixed_point_t a, fixed_point_t b)
{
    fixed_point_t result = a * b;
    result += 1UL << (FSHIFT - 1);
    result >>= FSHIFT;
    return result;
}
/**
 * return a / b in fixed-point
 * take scale factor so fractions of the result can be kept
 */
fixed_point_t div(fixed_point_t a, fixed_point_t b)
{
    fixed_point_t result = (a << SCALE_FACTOR) / b;
    result = LOAD_FIXED(result);
    result >>= SCALE_FACTOR;
    return result;
}


int main(void)
{
    fixed_point_t x = 0b01010110 << (FSHIFT - 4);
    fixed_point_t y = 0b00010110 << (FSHIFT - 4);
    print_fixed(x);
    print_fixed(y);
    y = div(x, y);
    printf("x / y = %lu.%lu\n", LOAD_INT(y), LOAD_FRAC(y));
}