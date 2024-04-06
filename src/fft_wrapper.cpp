#include "fft_wrapper.hpp"
#include <fftw3.h>
/* ================================================================= */
/* PRIVATE FUNCTIONS */
/* ================================================================= */

/* ================================================================= */
/* PUBLIC FUNCTIONS */
/* ================================================================= */

void FFT_call(fft_input_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]) {}
void IFFT_call(fft_output_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]) {}
/* Function to wrap actual FFT call */
void FFT_wrapper(bool direction, fft_complex_t in[FFT_LENGTH],
                 fft_complex_t out[FFT_LENGTH]) {}