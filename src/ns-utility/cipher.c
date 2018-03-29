//
// Created by wfxzz on 3/27/18.
//

#include "cipher.h"

byte *encode_map = NULL;
byte *decode_map = NULL;

byte map_encode(byte b) {
    return encode_map[b];
}

byte map_decode(byte b) {
    return decode_map[b];
}

byte *get_random_map() {
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

void get_encode_decode_map(byte const *map) {
    if (encode_map || decode_map) {
        fprintf(stderr, "encode/decode map already init");
        return;
    }

    encode_map = malloc(sizeof(byte) * KEY_LENGTH);
    decode_map = malloc(sizeof(byte) * KEY_LENGTH);

    int i;
    for (i = 0; i < KEY_LENGTH; ++i) {
        encode_map[i] = map[i];
        decode_map[map[i]] = i;
    }
}

size_t map2cipher() {
    size_t  cipher_length;
    byte *map = get_random_map();
    byte *cipher;

    cipher = (byte *)base64_encode((unsigned char *)map, KEY_LENGTH, &cipher_length);

    write(1, cipher, cipher_length);

    // cipher2map(cipher, cipher_length);

    return cipher_length;
}

byte *cipher2map(byte *cipher, size_t cipher_length) {
    int map_length;
    byte *map = (byte *)base64_decode((char *)cipher, cipher_length, &map_length);

    return map;
}

struct js easy_paser(char *pos) {
    struct js info;
    bzero(&info, sizeof(info));

    FILE *fp;
    fp = fopen(pos, "r");

    char buf[4096];

    fscanf(fp, "%s", info.cipher);
    fscanf(fp, "%s", info.local_address);
    fscanf(fp, "%s", info.local_port);
    fscanf(fp, "%s", info.server_address);
    fscanf(fp, "%s", info.server_port);

    fclose(fp);
    return info;
}

int main(int argc, char **argv) {
    map2cipher();
    return 0;
}