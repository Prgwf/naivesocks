//
// Created by wfxzz on 3/27/18.
//

#include "client.h"

struct sockaddr_storage get_local_info();
struct sockaddr_stroage get_remote_info();

void usage() {
    printf("usage:\n");
    printf("-c config file path\n");
    printf("-d daemon (todo)\n");
}

void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx);
void local_read_cb(struct bufferevent *bev, void *ctx);
void server_read_cb(struct bufferevent *bev, void *ctx);
void error_cb(struct bufferevent *bev, short error, void *ctx);

int main(int argc, char **argv) {
    int opt;
    char config_file_pos[128];
    bzero(&config_file_pos, sizeof(config_file_pos));

    while ((opt=getopt(argc,argv, "c:")) != -1) {
        switch(opt) {
            case 'c':
                sprintf(config_file_pos, "%s", optarg);
                break;
            default:
                usage();
                break;
        }
    }

    struct js config = easy_paser(config_file_pos);
    size_t length = strlen(config.cipher);
    byte *map = cipher2map((byte *)config.cipher, length);

    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in local_addr;

    bzero(&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(config.local_port));
    inet_aton(config.local_address, &local_addr.sin_addr);

    base = event_base_new();
    listener = evconnlistener_new_bind(base, do_accept, base,
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                       (struct sockaddr *)&local_addr, sizeof(local_addr));

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}


void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx) {
    struct event_base *base;
    struct bufferevent *bufevin, *bufevout;

    base = (struct event_base *)ctx;

    bufevin = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufevout = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE); // here: fd = -1 means set later...

    if (bufferevent_socket_connect(bufevout, addr, length) < 0) {
        perror("client: bufferevent_socket_connect!");
        bufferevent_free(bufevin);
        bufferevent_free(bufevout);
        return;
    }

    bufferevent_setcb(bufevout, server_read_cb, NULL, error_cb, bufevin);
    bufferevent_setcb(bufevin, local_read_cb, NULL, error_cb, bufevout);

    bufferevent_enable(bufevout, EV_READ|EV_WRITE);
    bufferevent_enable(bufevin, EV_READ|EV_WRITE);

}

// todo
void error_cb(struct bufferevent *bev, short error, void *ctx) {
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        /* ... */
    }
    bufferevent_free(bev);
}
