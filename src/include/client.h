//
// Created by wfxzz on 3/27/18.
//

#ifndef NAIVESOCKS_CLIENT_H
#define NAIVESOCKS_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>

#include "cipher.h"

struct Reomte {
    struct sockaddr_in sin;
    struct event_base *base;
};

void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx);
void read_cb_from_local(struct bufferevent *bev, void *ctx);
void read_cb_from_server(struct bufferevent *bev, void *ctx);
void error_cb(struct bufferevent *bev, short error, void *ctx);
void error_cb_from_local(struct bufferevent *bev, short what, void *ctx);
void error_cb_from_server(struct bufferevent *bev, short what, void *ctx);
void close_on_finish_write_cb(struct bufferevent *bev, void *ctx);


#endif //NAIVESOCKS_CLIENT_H
