#pragma once

#include "common_defs.hpp"


#define PSO_ITERATIONS 400 // 400
#define PSO_SWARM_SIZE 400 // 400

void pso_process(ap_fixed_64p32 args_estimate[ARGS_SIZE],
                 const sModelParams &params_ref,
                 const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]);
