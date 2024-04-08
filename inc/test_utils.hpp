#pragma once

#include "common_defs.hpp"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>

class test_utils {
  private:
    void print_name(const char *name);

  public:
    void file_read_float(const char *filename, float *data, int size);
    void file_read_float(const char *filename, ap_fixed_64p32 *data, int size);
    void file_read_complex(const char *filename_re, const char *filename_im,
                           ap_complex_32p16 *data, int size);
    void file_read_complex(const char *filename_re, const char *filename_im,
                           ap_complex_64p32 *data, int size);

    void print_float(const char *name, float *data, int size);
    void print_float(const char *name, ap_fixed_64p32 *data, int size);
    void print_complex(const char *name, ap_complex_32p16 *data, int size);
    void print_complex(const char *name, ap_complex_64p32 *data, int size);
};

void test_utils::print_name(const char *name) {
    printf("========================================\n");
    printf("%s:\n", name);
    printf("========================================\n");
}

void test_utils::file_read_float(const char *filename, float *data, int size) {
    std::ifstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < size; i++) {
            if (file >> data[i]) {
                // Successfully read a float value from the file
            } else {
                printf("Failed to read float value from file: %s\n", filename);
                return;
            }
        }
        file.close();
    } else {
        printf("Failed to open file: %s\n", filename);
    }
}
void test_utils::file_read_float(const char *filename, ap_fixed_64p32 *data,
                                 int size) {
    std::ifstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < size; i++) {
            if (file >> data[i]) {
                // Successfully read a ap_fixed_64p32 value from the file
            } else {
                printf("Failed to read ap_fixed_64p32 value from file: %s\n",
                       filename);
                return;
            }
        }
        file.close();
    } else {
        printf("Failed to open file: %s\n", filename);
    }
}
void test_utils::file_read_complex(const char *filename_re,
                                   const char *filename_im,
                                   ap_complex_64p32 *data, int size) {
    std::ifstream file_re(filename_re);
    std::ifstream file_im(filename_im);
    if (file_re.is_open() && file_im.is_open()) {
        for (int i = 0; i < size; i++) {
            ap_fixed_64p32 re, im;
            if ((file_re >> re) && (file_im >> im)) {
                data[i] = std::complex<ap_fixed_64p32>(re, im);
            } else {
                printf("Failed to read complex value from files: %s, %s\n",
                       filename_re, filename_im);
                return;
            }
        }
        file_re.close();
        file_im.close();
    } else {
        printf("Failed to open files: %s, %s\n", filename_re, filename_im);
    }
}

void test_utils::print_float(const char *name, ap_fixed_64p32 *data, int size) {
    print_name(name);
    for (int i = 0; i < size; i++) {
        printf("%f\n", (float)data[i]);
    }
    printf("\n");
}
void test_utils::print_complex(const char *name, ap_complex_64p32 *data,
                               int size) {
    print_name(name);
    for (int i = 0; i < size; i++) {
        printf("%f + i%f\n", (float)data[i].real(), (float)data[i].imag());
    }
    printf("\n");
}
