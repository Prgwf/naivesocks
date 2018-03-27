//
// Created by wfxzz on 3/27/18.
//

#ifndef NAIVESOCKS_CIPHER_H
#define NAIVESOCKS_CIPHER_H


#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define KEY_LENGTH 256
typedef uint8_t byte;

byte encode(byte b);
byte decode(byte b);

byte *rand_password();
void new_chipher(byte *dict);


#endif //NAIVESOCKS_CIPHER_H

