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
    fscanf(fp, "%s", info.server_address);
    fscanf(fp, "%s", info.server_port);
    fscanf(fp, "%s", info.local_address);
    fscanf(fp, "%s", info.local_port);

    fclose(fp);
    return info;
}

int bufferevent_write_encode(struct bufferevent *bev,
                 void *ctx,
                 size_t length) {
    /* encode the data before writo into the buffer*/
    int i, n;
    byte *buf = ctx;

    for (i = 0; i < length; ++i) {
        buf[i] = map_encode(buf[i]);
    }

    n = bufferevent_write(bev, buf, length);

    return n;
}

int bufferevent_read_decode(struct bufferevent *bev,
                void *ctx,
                size_t length) {
    /* decode the data after read from the buffer*/

    int i;
    byte *buf = ctx;

    size_t  n = bufferevent_read(bev, buf, length);

    if (n > 0) {
        for (i = 0; i < length; ++i) {
            buf[i] = map_decode(buf[i]);
        }
    }

    return (int)n;
}

int encode_copy(struct bufferevent *bev,
                struct evbuffer *input) {

    struct evbuffer_iovec *v;  /* io optimization */
    size_t size = evbuffer_get_length(input);

    int i, j, n;
    n = evbuffer_peek(input, size, NULL, NULL, 0);
    if (n > 0) {
        v = malloc(sizeof(struct evbuffer_iovec) * n);
        n = evbuffer_peek(input, size, NULL, v, n);
        for (i = 0; i < n; ++i) {
            byte *b = v[i].iov_base;
            size_t blen = v[i].iov_len;
            for (j = 0; j < blen; ++j) {
                b[j] = map_encode(b[j]);
            }
        }

        free(v);

        bufferevent_write_buffer(bev, input);
    }

    return n;
}

int decode_copy(struct bufferevent *bev,
                struct evbuffer *output) {
    return 0;
}

