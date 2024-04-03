#include "common_defs.hpp"
#include "model.hpp"
#include "pso.hpp"
#include "test_utils.hpp"

void test_TransferFunction(void) {
    test_utils util;
    /* Read test input values */
    ap_fixed_64p32 in_args[ARGS_SIZE] = {0.0f};
    util.file_read_float(ARGS_DIR, &in_args[0], ARGS_SIZE);
    sModelArgs args = {in_args[0], in_args[1], in_args[2],
                       in_args[3], in_args[4], in_args[5]};

    ap_fixed_64p32 in_params[PARAMS_SIZE] = {0.0f};
    util.file_read_float(PARAMS_DIR, &in_params[0], PARAMS_SIZE);
    sModelParams params = {in_params[0], in_params[1], in_params[2]};

    ap_fixed_64p32 in_freq_axis[TRANSFER_FUNC_SIZE] = {0.0f};
    util.file_read_float(FREQ_AXIS_DIR, &in_freq_axis[0], TRANSFER_FUNC_SIZE);

    /* Read expected output values */
    ap_complex_64p32 tf_out[TRANSFER_FUNC_SIZE];
    util.file_read_complex(TRANSFER_FUN_RE_DIR, TRANSFER_FUN_IM_DIR, &tf_out[0],
                           TRANSFER_FUNC_SIZE);

    /* Call the model */
    ap_complex_32p16 tf_result[TRANSFER_FUNC_SIZE];
    TransferFunction(args, params, in_freq_axis, tf_result);

    for (int i = 0; i < TRANSFER_FUNC_SIZE; i++) {
        ap_complex_64p32 T = tf_result[i];
        PRINT_C(T, i);
    }
}

void test_WaveSynthesis(void) {
    test_utils util;
    /* Read test input values */
    ap_fixed_64p32 in_args[ARGS_SIZE] = {0.0f};
    util.file_read_float(ARGS_DIR, &in_args[0], ARGS_SIZE);
    sModelArgs args = {in_args[0], in_args[1], in_args[2],
                       in_args[3], in_args[4], in_args[5]};

    ap_fixed_64p32 in_params[PARAMS_SIZE] = {0.0f};
    util.file_read_float(PARAMS_DIR, &in_params[0], PARAMS_SIZE);
    sModelParams params = {in_params[0], in_params[1], in_params[2]};

    ap_fixed_64p32 in_freq_axis[TRANSFER_FUNC_SIZE] = {0.0f};
    util.file_read_float(FREQ_AXIS_DIR, &in_freq_axis[0], TRANSFER_FUNC_SIZE);

    ap_fixed_32p16 in_refrence[TRANSFER_FUNC_SIZE];
    util.file_read_float(REFRENCE_DIR, &in_refrence[0], TRANSFER_FUNC_SIZE);

    /* Read expected output values */
    ap_fixed_32p16 wave_out[TRANSFER_FUNC_SIZE];
    util.file_read_float(WAVE_DIR, &wave_out[0], TRANSFER_FUNC_SIZE);

    /* Call the model */
    ap_fixed_32p16 wave_result[TRANSFER_FUNC_SIZE];
    WaveSynthesis(args, params, in_refrence, in_freq_axis, wave_result);

    for (int i = 0; i < TRANSFER_FUNC_SIZE; i++) {
        ap_fixed_32p16 wave_out = wave_result[i];
        PRINT(wave_out, i);
    }
}

void test_pso_process(void) {
    test_utils util;
    /* Read test input values */
    ap_fixed_64p32 in_args[ARGS_SIZE] = {0.0f};
    util.file_read_float(ARGS_DIR, &in_args[0], ARGS_SIZE);
    sModelArgs args = {in_args[0], in_args[1], in_args[2],
                       in_args[3], in_args[4], in_args[5]};

    ap_fixed_64p32 in_params[PARAMS_SIZE] = {0.0f};
    util.file_read_float(PARAMS_DIR, &in_params[0], PARAMS_SIZE);
    sModelParams params = {in_params[0], in_params[1], in_params[2]};

    ap_fixed_64p32 in_freq_axis[TRANSFER_FUNC_SIZE] = {0.0f};
    util.file_read_float(FREQ_AXIS_DIR, &in_freq_axis[0], TRANSFER_FUNC_SIZE);

    ap_fixed_32p16 in_refrence[TRANSFER_FUNC_SIZE];
    util.file_read_float(REFRENCE_DIR, &in_refrence[0], TRANSFER_FUNC_SIZE);

    ap_fixed_32p16 in_meas_signal[TRANSFER_FUNC_SIZE];
    util.file_read_float(MEAS_DIR, &in_meas_signal[0], TRANSFER_FUNC_SIZE);

    /* Call the model */
    // sModelArgs args_estimate = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    ap_fixed_64p32 args_estimate[PARAMS_SIZE] = {0.0f};
    pso_process(args_estimate, params, in_meas_signal, in_refrence, in_freq_axis, 100);
}

int main() {
    // test_TransferFunction();
    // test_WaveSynthesis();
    test_pso_process();
    return 0;
}
