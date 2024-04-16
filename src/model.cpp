#include "model.hpp"
#include <stdio.h>
#include "fft_wrapper.hpp"

static const ap_fixed_32p16 PI_x_2 = 6.28318530717958647692;

void TransferFunction_single(const sModelArgs &args, const sModelParams &params,
                             const ap_fixed_64p32 &freq_axis,
                             ap_complex_32p16 &tf_out) {
    const ap_complex_32p16 imag_unit = ap_complex_32p16(0, 1);

    ap_fixed_32p16 alf = 0;
    ap_fixed_32p16 alf_1 = 0;
    ap_fixed_32p16 alf_2 = 0;

    /* k - wave number */
    ap_fixed_32p16 k_real = 0;
    ap_fixed_32p16 k_imag = 0;

    ap_fixed_32p16 Z1 = params.c1 * params.ro1;
    // PRINT(Z1, 0);
    ap_fixed_32p16 Z1_dec = Z1;
    // PRINT(Z1_dec, 0);
    ap_fixed_64p32 Z2 = args.c2 * args.ro2;
    // PRINT(Z2, 0);
    ap_fixed_32p16 Z2_dec = Z2;
    // PRINT(Z2_dec, 0);

    ap_fixed_64p32 numerator = Z1 * Z2_dec; // DECIMATED!
    // PRINT(numerator, 0);

    ap_fixed_32p16 denominator_sum_sqr = Z1 * Z1_dec + Z2 * Z2_dec; // DECIMATED!
    // PRINT(denominator_sum_sqr, 0);

    ap_fixed_32p16 real_kh = 0;
    ap_fixed_32p16 imag_kh = 0;

    ap_fixed_32p16 cos_kh_real = 0;
    ap_fixed_32p16 cos_kh_imag = 0;
    ap_complex_32p16 cos_kh;

    ap_fixed_32p16 sin_kh_real = 0;
    ap_fixed_32p16 sin_kh_imag = 0;
    ap_complex_32p16 sin_kh;

    ap_complex_64p32 denominator;
    ap_complex_64p32 T;

    // tf_out[0] = 0;

    /* Calculate alfa - attenuation coefficient */
    alf_1 = abs(freq_axis) / args.freq0;
    // PRINT(alf_1, i);
    alf_2 = pow((float)alf_1, (float)args.n);
    // alf_2 = alf_1 ^ args.n;
    // PRINT(alf_2, i);
    alf = args.alfa0 * alf_2;
    // PRINT(alf, i);

    /* Calculate k - wave number */
    // PRINT(freq_axis[i], i);
    k_real = PI_x_2 * (ap_fixed_32p16)(freq_axis / args.c2);
    k_imag = alf;
    // PRINT(k_real, i);
    // PRINT(k_imag, i);

    /* Calculate transfer function */
    /* ================================================================= */
    /* Brekhovskikh & Godin 1990 (2.4.18) */
    /* T =  (2 * Z1 * Z2) ./
            ((2 * Z1 * Z2) .* cos(k.*h) - 1i .* sin(k.*h) .* (Z1.^2 + Z2.^2));
     */
    /* ================================================================= */

    real_kh = k_real * args.h;
    imag_kh = k_imag * args.h;
    // PRINT(real_kh, i);
    // PRINT(imag_kh, i);

    /* Apply Euler's Cosine Identity replacement */
    cos_kh_real = cos(real_kh) * cosh(imag_kh);
    cos_kh_imag = -1 * sin(real_kh) * sinh(imag_kh);
    cos_kh = ap_complex_32p16(cos_kh_real, cos_kh_imag);
    // PRINT_C(cos_kh, i);

    /* Apply Euler's Sine Identity replacement */
    sin_kh_real = sin(real_kh) * cosh(imag_kh);
    sin_kh_imag = cos(real_kh) * sinh(imag_kh);
    sin_kh = ap_complex_32p16(sin_kh_real, sin_kh_imag);
    // PRINT_C(sin_kh, i);

    denominator = ((ap_fixed_32p16)numerator * cos_kh) -
                  (imag_unit * sin_kh * denominator_sum_sqr); // DECIMATED!
    // PRINT_C(denominator, i);

    T = numerator / denominator;
    // PRINT_C(T, i);
    tf_out = T;
}

void TransferFunction(const sModelArgs args, const sModelParams params,
                      const ap_fixed_64p32 *freq_axis, ap_complex_32p16 *tf_out) {
    
    const ap_complex_32p16 imag_unit = ap_complex_32p16(0, 1);

    ap_fixed_32p16 alf = 0;
    ap_fixed_32p16 alf_1 = 0;
    ap_fixed_32p16 alf_2 = 0;

    /* k - wave number */
    ap_fixed_32p16 k_real = 0;
    ap_fixed_32p16 k_imag = 0;

    ap_fixed_32p16 Z1 = params.c1 * params.ro1;
    // PRINT(Z1, 0);
    ap_fixed_32p16 Z1_dec = Z1;
    // PRINT(Z1_dec, 0);
    ap_fixed_64p32 Z2 = args.c2 * args.ro2;
    // PRINT(Z2, 0);
    ap_fixed_32p16 Z2_dec = Z2;
    // PRINT(Z2_dec, 0);

    ap_fixed_64p32 numerator = Z1 * Z2_dec; // DECIMATED!
    // PRINT(numerator, 0);

    ap_fixed_32p16 denominator_sum_sqr = Z1 * Z1_dec + Z2 * Z2_dec; // DECIMATED!
    // PRINT(denominator_sum_sqr, 0);

    ap_fixed_32p16 real_kh = 0;
    ap_fixed_32p16 imag_kh = 0;

    ap_fixed_32p16 cos_kh_real = 0;
    ap_fixed_32p16 cos_kh_imag = 0;
    ap_complex_32p16 cos_kh;

    ap_fixed_32p16 sin_kh_real = 0;
    ap_fixed_32p16 sin_kh_imag = 0;
    ap_complex_32p16 sin_kh;

    ap_complex_64p32 denominator;
    ap_complex_64p32 T;

    tf_out[0] = ap_complex_32p16(0, 0);
    for (int i = 1; i < TRANSFER_FUNC_SIZE; i++) {
        /* Calculate alfa - attenuation coefficient */
        alf_1 = abs(freq_axis[i]) / args.freq0;
        // PRINT(alf_1, i);
        alf_2 = pow((float)alf_1, (float)args.n);
        // alf_2 = alf_1 ^ args.n;
        // PRINT(alf_2, i);
        alf = args.alfa0 * alf_2;
        // PRINT(alf, i);

        /* Calculate k - wave number */
        // PRINT(freq_axis[i], i);
        k_real = PI_x_2 * (ap_fixed_32p16)(freq_axis[i] / args.c2);
        k_imag = alf;
        // PRINT(k_real, i);
        // PRINT(k_imag, i);

        /* Calculate transfer function */
        /* ================================================================= */
        /* Brekhovskikh & Godin 1990 (2.4.18) */
        /* T =  (2 * Z1 * Z2) ./ 
                ((2 * Z1 * Z2) .* cos(k.*h) - 1i .* sin(k.*h) .* (Z1.^2 + Z2.^2)); */
        /* ================================================================= */

        real_kh = k_real * args.h;
        imag_kh = k_imag * args.h;
        // PRINT(real_kh, i);
        // PRINT(imag_kh, i);

        /* Apply Euler's Cosine Identity replacement */
        cos_kh_real = cos(real_kh) * cosh(imag_kh);
        cos_kh_imag = -1 * sin(real_kh) * sinh(imag_kh);
        cos_kh = ap_complex_32p16(cos_kh_real, cos_kh_imag);
        // PRINT_C(cos_kh, i);

        /* Apply Euler's Sine Identity replacement */
        sin_kh_real = sin(real_kh) * cosh(imag_kh);
        sin_kh_imag = cos(real_kh) * sinh(imag_kh);
        sin_kh = ap_complex_32p16(sin_kh_real, sin_kh_imag);
        // PRINT_C(sin_kh, i);

        denominator = ((ap_fixed_32p16)numerator * cos_kh) - (imag_unit * sin_kh * denominator_sum_sqr); // DECIMATED!
        // PRINT_C(denominator, i);

        T = numerator / denominator;
        // PRINT_C(T, i);
        tf_out[i] = T;
    }
}

void WaveSynthesis(const sModelArgs args, const sModelParams params,
                   const ap_fixed_32p16 *ref_signal,
                   const ap_fixed_64p32 *freq_axis, ap_fixed_32p16 *wave_out) {

    const ap_complex_32p16 imag_unit = ap_complex_32p16(0, -1);

    /* Model Transfer function */
    ap_complex_32p16 T[TRANSFER_FUNC_SIZE];
    // TransferFunction(args, params, freq_axis, T);

    /* Model Response spectrum */
    ap_complex_32p16 input_fft[TRANSFER_FUNC_SIZE];
    FFT_call((fft_input_t *)ref_signal, input_fft);
    // PRINT_ARRAY_C(input_fft, TRANSFER_FUNC_SIZE);

    ap_complex_32p16 T_conj;
    ap_fixed_32p16 exp_arg;
    ap_fixed_32p16 exp_sub_real;
    ap_fixed_32p16 exp_sub_imag;
    ap_complex_32p16 exp_sub;
    ap_complex_32p16 resp_spectrum;
    ap_complex_32p16 response_spectrum[TRANSFER_FUNC_SIZE];
    ap_complex_32p16 wave_complex;
    response_spectrum[0] = ap_complex_32p16(0, 0);
    for (int i = 1; i < TRANSFER_FUNC_SIZE; i++) {
#pragma HLS pipeline
        TransferFunction_single(args, params, freq_axis[i], T[i]);
        T_conj = ap_complex_32p16(T[i].real(), -1 * T[i].imag());
        // PRINT_C(T_conj, i);
        exp_arg = PI_x_2 * (ap_fixed_32p16)(freq_axis[i] * args.h) / params.c1;
        // PRINT(exp_arg, i);
        exp_sub_real = cos(exp_arg);
        exp_sub_imag = sin(exp_arg);
        exp_sub = ap_complex_32p16(exp_sub_real, exp_sub_imag);
        // PRINT_C(exp_sub, i);
        response_spectrum[i] = input_fft[i] * T_conj * exp_sub;
        resp_spectrum = response_spectrum[i];
        // PRINT_C(resp_spectrum, i);
    }

    IFFT_call(response_spectrum, wave_out);
    // PRINT_ARRAY(wave_out, TRANSFER_FUNC_SIZE);
}
