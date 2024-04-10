#pragma once

#include "common_defs.hpp"

struct sArgsRange_t {
    ap_fixed_64p32 min;
    ap_fixed_64p32 max;
};

struct sParticleCfg_t {
    ap_fixed_32p16 inertia;
    ap_fixed_32p16 personal_weight;
    ap_fixed_32p16 global_weight;
};

#ifdef __FPGA__

/* PRNG */
/* Xorshift16 */
static uint16_t RNG16 = 12345;
uint16_t xorshift16() {
    RNG16 ^= (RNG16 << 7);
    RNG16 ^= (RNG16 >> 9);
    RNG16 ^= (RNG16 << 8);
    return RNG16;
}
ap_fixed_32p16 PRNG_16() {
    ap_fixed_32p16 randomFloat = (xorshift16() & 0x7FFF);
    randomFloat = randomFloat >> 15;
    return randomFloat;
}

/* Xorshift32 */
static uint32_t RNG32 = 123456789;
uint32_t xorshift32() {
    RNG32 ^= (RNG32 << 13);
    RNG32 ^= (RNG32 >> 17);
    RNG32 ^= (RNG32 << 5);
    return RNG32;
}
ap_fixed_32p16 PRNG_32() {
    ap_fixed_32p16 randomFloat = (xorshift32() & 0x7FFFFFFF);
    randomFloat = randomFloat >> 31;
    return randomFloat;
}

/* Xorshift64 */
static uint64_t RNG64 = 123456789123456789;
uint64_t xorshift64() {
    RNG64 ^= (RNG64 << 13);
    RNG64 ^= (RNG64 >> 7);
    RNG64 ^= (RNG64 << 17);
    return RNG64;
}
ap_fixed_32p16 PRNG_64() {
    ap_fixed_32p16 randomFloat = (xorshift64() & 0x7FFFFFFFFFFFFFFF);
    randomFloat = randomFloat >> 63;
    return randomFloat;
}
ap_fixed_64p32 PRNG_64p32(const uint64_t &max_mask) {
    ap_fixed_64p32 randomFloat;
    randomFloat.range(63, 0) = (xorshift64() & max_mask);
    return randomFloat;
}
#else

ap_fixed_64p32 PRNG_64p32() {
    ap_fixed_64p32 randomFloat = (ap_fixed_64p32)rand() / RAND_MAX;
    return randomFloat;
}

#endif

void pso_util_print(const char *text, int i, ap_fixed_64p32 *args) {
    printf("%s[%-4d]: alfa0=%-10f; freq0=%-10f; c2=%-10f; n=%-10f; ro2=%-10f; h=%-10f;\n", text, i,
           (double)args[0], (double)args[1], (double)args[2], (double)args[3],
              (double)args[4], (double)args[5]);
}

void pso_util_print(const char *text, ap_fixed_64p32 *args) {
    printf("[PSO] %s: alfa0=%-10f, freq0=%-10f, c2=%-10f, n=%-10f, ro2=%-10f, h=%-10f\n", text,
           (double)args[0], (double)args[1], (double)args[2], (double)args[3],
              (double)args[4], (double)args[5]);
}
