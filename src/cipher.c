//
// Created by wfxzz on 3/27/18.
//

#include <stdbool.h>
#include <stdio.h>
#include "cipher.h"

byte *encode_dict = NULL;
byte *decode_dict = NULL;

byte encode(byte b) {
    return encode_dict[b];
}

byte decode(byte b) {
    return decode_dict[b];
}

byte *rand_password() {
    /* one to one mapped*/
    byte *a = malloc(sizeof(byte) * KEY_LENGTH);

    for (; ; ) {
        int i, j;
        byte k = 0;
        for (i = 0; i < KEY_LENGTH; ++i) {
            a[i] = k++;
        }

        /* random shuffle*/
        for (i = 0; i < KEY_LENGTH; ++i) {
            j = rand() % KEY_LENGTH;

            byte t = a[i];
            a[i] = a[j];
            a[j] = t;
        }

        /* check if all a[i] != i */
        bool flag = 1;
        for (i = 0; i < KEY_LENGTH; ++i) {
            if (i == a[i]) {
                flag = 0;
            }
        }
        if (flag) break;
    }
    return a;
}

void new_chiper(byte *dict) {
    if (encode_dict || decode_dict) {
        fprintf(stderr, "encode dict or decode dict already init");
        return;
    }

    int i;
    for (i = 0; i < KEY_LENGTH; ++i) {
        encode_dict[i] = dict[i];
        decode_dict[dict[i]] = i;
    }

}

int main() {
}