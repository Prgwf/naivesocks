//
// Created by wfxzz on 3/27/18.
//

#ifndef NAIVESOCKS_CIPHER_H
#define NAIVESOCKS_CIPHER_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <zconf.h>
#include <string.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "base64.h"


#define KEY_LENGTH 256
typedef uint8_t byte;

byte map_encode(byte b);
byte map_decode(byte b);

void get_encode_decode_map(byte const *map);

byte *get_random_map();
size_t map2cipher();
byte *cipher2map(byte *cipher, size_t length);

struct js {
    char cipher[2048];
    char local_address[32];
    char local_port[16];
    char server_address[32];
    char server_port[16];
} ;

struct js easy_paser(char *pos);

int encode_write(struct bufferevent *bev, void *data, size_t length);
int decode_read(struct bufferevent *bev, void *data, size_t length);
int encode_copy(struct bufferevent *bev, struct evbuffer *input);
int decode_copy(struct bufferevent *bev, struct evbuffer *output);


#endif //NAIVESOCKS_CIPHER_H

