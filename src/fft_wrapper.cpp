#include "fft_wrapper.hpp"
#include <fftw3.h>
/* ================================================================= */
/* PRIVATE FUNCTIONS */
/* ================================================================= */

// Global FFTW plans for reuse
static fftwf_plan forward_plan;
static fftwf_plan inverse_plan;

/* ================================================================= */
/* PUBLIC FUNCTIONS */
/* ================================================================= */

void FFT_call(fft_input_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]) {
    fftwf_complex *in_fftw =
        (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * FFT_LENGTH);
    fftwf_complex *out_fftw =
        (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * FFT_LENGTH);

    for (int i = 0; i < FFT_LENGTH; i++) {
        in_fftw[i][0] = in[i];
        in_fftw[i][1] = 0;
    }

    if (forward_plan == NULL) {
        forward_plan = fftwf_plan_dft_1d(FFT_LENGTH, in_fftw, out_fftw,
                                         FFTW_FORWARD, FFTW_ESTIMATE);
    }

    fftwf_execute(forward_plan);

    for (int i = 0; i < FFT_LENGTH; i++) {
        out[i] = fft_output_t(out_fftw[i][0], out_fftw[i][1]);
    }

    fftwf_free(in_fftw);
    fftwf_free(out_fftw);
}
void IFFT_call(fft_output_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]) {
    fftwf_complex *in_fftw =
        (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * FFT_LENGTH);
    fftwf_complex *out_fftw =
        (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * FFT_LENGTH);

    for (int i = 0; i < FFT_LENGTH; i++) {
        in_fftw[i][0] = in[i].real();
        in_fftw[i][1] = in[i].imag();
    }

    if (inverse_plan == NULL) {
        inverse_plan = fftwf_plan_dft_1d(FFT_LENGTH, in_fftw, out_fftw,
                                         FFTW_BACKWARD, FFTW_ESTIMATE);
    }

    fftwf_execute(inverse_plan);

    for (int i = 0; i < FFT_LENGTH; i++) {
        out[i] = out_fftw[i][0] / FFT_LENGTH;
    }

    fftwf_free(in_fftw);
    fftwf_free(out_fftw);
}

/* Function to wrap actual FFT call */
void FFT_wrapper(bool direction, fft_complex_t in[FFT_LENGTH],
                 fft_complex_t out[FFT_LENGTH]) {}