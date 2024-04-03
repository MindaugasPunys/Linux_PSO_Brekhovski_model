#pragma once

#include "common_defs.hpp"
#include "hls_fft.h"

using namespace std;

/* ================================================================= */
/* FFT configuration */
/* ================================================================= */

const char FFT_WIDTH = 32;
const char FFT_CONFIG_WIDTH = 16;
const char FFT_NFFT_MAX = 10;
const int FFT_LENGTH = 1 << FFT_NFFT_MAX;

struct config1 : hls::ip_fft::params_t {
    static const unsigned input_width = FFT_WIDTH;
    static const unsigned output_width = FFT_WIDTH;
    static const unsigned ordering_opt = hls::ip_fft::natural_order;
    static const unsigned config_width = FFT_CONFIG_WIDTH;
    static const unsigned max_nfft = FFT_NFFT_MAX;
};
typedef hls::ip_fft::config_t<config1> config_t;
typedef hls::ip_fft::status_t<config1> status_t;

/* ================================================================= */
/* FFT input/output types */
/* ================================================================= */
typedef ap_fixed<32, 16> fft_input_t;
typedef std::complex<fft_input_t> fft_output_t;

typedef ap_fixed<FFT_WIDTH, 1> fft_float_t;
typedef std::complex<fft_float_t> fft_complex_t;

/* ================================================================= */
/* Public functions */
/* ================================================================= */
void FFT_call(fft_input_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]);
void IFFT_call(fft_output_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]);
void FFT_wrapper(bool direction, fft_complex_t in[FFT_LENGTH],	fft_complex_t out[FFT_LENGTH]);
