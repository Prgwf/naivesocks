//
// Created by wfxzz on 3/28/18.
//

#ifndef NAIVESOCKS_SECURESOCKET_H
#define NAIVESOCKS_SECURESOCKET_H

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "cipher.h"

int encode_write(struct bufferevent *bev, void *data, size_t length);
int decode_read(struct bufferevent *bev, void *data, size_t length);
int encode_copy(struct bufferevent *bev, struct evbuffer *input);
int decode_copy(struct bufferevent *bev, struct evbuffer *output);


#endif //NAIVESOCKS_SECURESOCKET_H
