#include "fft_wrapper.hpp"

/* ================================================================= */
/* PRIVATE FUNCTIONS */
/* ================================================================= */

static const uint32_t input_shift = (fft_input_t::iwidth - 3); // Same as division from max value
static const uint32_t output_shift = (fft_input_t::iwidth - 8); // Same as multiplication from max value
/* Function to scale ap_fixed<32, 16> to complex<16, 1>
 * Example: Scale +-32,767 to +-1 */
void FFT_input_to_complex(fft_input_t in[FFT_LENGTH], fft_complex_t out[FFT_LENGTH]) {
	fft_input_t temp_normalised = 0;
	for (int i = 0; i < FFT_LENGTH; i++) {
        temp_normalised = in[i] >> FFT_NFFT_MAX + 1;
		out[i] = fft_complex_t(temp_normalised, 0);
	}
}
/* Function to scale complex<32, 16> to complex<16, 1>
 * Scale +-32,767i to +-1i */
void FFT_output_to_complex(fft_output_t in[FFT_LENGTH], fft_complex_t out[FFT_LENGTH]) {
	static fft_output_t temp_val;
	fft_input_t temp_real = 0;
	fft_input_t temp_imag = 0;
	for (int i = 0; i < FFT_LENGTH; i++) {
		temp_real = in[i].real();
        temp_real = temp_real >> FFT_NFFT_MAX - 2;
		temp_imag = in[i].imag();
		temp_imag = temp_imag >> FFT_NFFT_MAX - 2;
		out[i] = fft_complex_t(temp_real, temp_imag);
	}
}
/* Function to scale complex<16, 1> to ap_fixed<32, 16>
 * Scale back +-1i to +-32,767
 * Scale back FFT 2N reduction*/
void FFT_complex_to_input(fft_complex_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]) {
	static fft_input_t temp_val;
	fft_complex_t temp_complex;
	fft_input_t temp_normalised;
	for (int i = 0; i < FFT_LENGTH; i++) {
		temp_complex = in[i];
		temp_normalised = temp_complex.real();
		temp_normalised = temp_normalised << FFT_NFFT_MAX - 1;
		out[i] = temp_normalised;
	}
}
/* Function to scale complex<16, 1> to complex<32, 16>
 * Scale back +-1i to +-32,767i
 * Scale back FFT 2N reduction*/
void FFT_complex_to_output(fft_complex_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]) {
	static fft_output_t temp_val;
	fft_complex_t temp_complex;
    fft_input_t temp_real;
    fft_input_t temp_imag;
	for (int i = 0; i < FFT_LENGTH; i++) {
		temp_complex = in[i];
		temp_real = temp_complex.real();
		temp_real = temp_real << (2 * FFT_NFFT_MAX + 2);
		temp_imag = temp_complex.imag();
		temp_imag = temp_imag << (2 * FFT_NFFT_MAX + 2);
		out[i] = fft_output_t(temp_real, temp_imag);
	}
}

/* ================================================================= */
/* PUBLIC FUNCTIONS */
/* ================================================================= */

void FFT_call(fft_input_t in[FFT_LENGTH], fft_output_t out[FFT_LENGTH]) {
	fft_complex_t fft_in[FFT_LENGTH];
	fft_complex_t fft_out[FFT_LENGTH];

	FFT_input_to_complex(in, fft_in);
	FFT_wrapper(true, fft_in, fft_out);
	FFT_complex_to_output(fft_out, out);
}
void IFFT_call(fft_output_t in[FFT_LENGTH], fft_input_t out[FFT_LENGTH]) {
	fft_complex_t fft_in[FFT_LENGTH];
	fft_complex_t fft_out[FFT_LENGTH];

	FFT_output_to_complex(in, fft_in);
	FFT_wrapper(false, fft_in, fft_out);
	FFT_complex_to_input(fft_out, out);
}
/* Function to wrap actual FFT call */
void FFT_wrapper(bool direction, fft_complex_t in[FFT_LENGTH],	fft_complex_t out[FFT_LENGTH]) {
	
	status_t fft_status;
	config_t fft_config;

	fft_config.setDir(direction);
	fft_config.setSch(0x2AB);

#pragma HLS dataflow
#pragma HLS INLINE recursive
    hls::fft<config1>(in, out, &fft_status, &fft_config);
}