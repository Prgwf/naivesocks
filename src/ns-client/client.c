//
// Created by wfxzz on 3/27/18.
//

#include "client.h"

struct sockaddr_storage get_local_info();

struct sockaddr_stroage get_remote_info();



int main(int args, int **argv) {
    byte *cipher;
    size_t length;
    byte *map = cipher2map(cipher, length);


    struct sockaddr_storage localaddr, remoteaddr;
    size_t localaddr_length, remoteaddr_length;

    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr addr;


}