#pragma once

#include "common_defs.hpp"
// #include "hls_fft.h"

using namespace std;

/* ================================================================= */
/* FFT configuration */
/* ================================================================= */

const char FFT_WIDTH = 32;
const char FFT_CONFIG_WIDTH = 16;
const char FFT_NFFT_MAX = 10;
const int FFT_LENGTH = 1 << FFT_NFFT_MAX;

/* ================================================================= */
/* FFT input/output types */
/* ================================================================= */
typedef float fft_input_t;
typedef std::complex<fft_input_t> fft_output_t;

typedef float fft_float_t;
typedef std::complex<fft_float_t> fft_complex_t;

/* ================================================================= */
/* Public functions */
/* ================================================================= */
void FFT_call(fft_input_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]);
void IFFT_call(fft_output_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]);
void FFT_wrapper(bool direction, fft_complex_t in[FFT_LENGTH],	fft_complex_t out[FFT_LENGTH]);
