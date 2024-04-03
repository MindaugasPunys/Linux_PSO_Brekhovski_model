#pragma once

#include "common_defs.hpp"

#define PSO_ITERATIONS 200
#define PSO_SWARM_SIZE 10

void pso_process(ap_fixed_64p32 args_estimate[PARAMS_SIZE],
                 const sModelParams &params,
                 const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE],
                 const uint16_t itterations);
