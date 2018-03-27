//
// Created by wfxzz on 3/27/18.
//

#ifndef NAIVESOCKS_BASE64_H
#define NAIVESOCKS_BASE64_H

#include <stdlib.h>
#include <stdint.h>

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length);
void base64_cleanup();

#endif //NAIVESOCKS_BASE64_H
