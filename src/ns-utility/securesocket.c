//
// Created by wfxzz on 3/28/18.
//
#include "securesocket.h"

int encode_write(struct bufferevent *bev,
                 void *data,
                 size_t length) {
    /* encode the data before writo into the buffer*/
    int i, n;
    byte *buf = data;

    for (i = 0; i < length; ++i) {
        buf[i] = map_encode(buf[i]);
    }

    n = bufferevent_write(bev, buf, length);

    return n;
}

int decode_read(struct bufferevent *bev,
                void *data,
                size_t length) {
    /* decode the data after read from the buffer*/

    int i;
    byte *buf = data;

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

int main() {
    printf("Hello World\n");


}