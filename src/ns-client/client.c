//
// Created by wfxzz on 3/27/18.
//

#include "client.h"

void usage() {
    printf("usage:\n");
    printf("-c config file path\n");
    printf("-d daemon (todo)\n");
}

void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx);
void read_cb_from_local(struct bufferevent *bev, void *ctx);
void read_cb_from_server(struct bufferevent *bev, void *ctx);
void error_cb(struct bufferevent *bev, short error, void *ctx);
void error_cb_from_local(struct bufferevent *bev, short what, void *ctx);
void error_cb_from_server(struct bufferevent *bev, short what, void *ctx);
void close_on_finish_write_cb(struct bufferevent *bev, void *ctx);



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

    // show some info
    printf("local addr: %s\n", config.local_address);
    printf("local port: %s\n", config.local_port);

    base = event_base_new();
    if (!base) {
        perror("base");
        return 1;
    }
    listener = evconnlistener_new_bind(base, do_accept, base,
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                       (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (!listener) {
        perror("listener");
        event_base_free(base);
        return 1;
    }

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_base_free(base);

    printf("Client finsh!\n");
    return 0;
}


void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx) {
    // debug by printf
    printf("in func: do_accept\n");

    struct event_base *base;
    struct bufferevent *bufevin, *bufevout;

    base = (struct event_base *)ctx;

    bufevin = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufevout = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE); // here: fd = -1 means set later...

    if (bufferevent_socket_connect(bufevout, addr, length) < 0) {
        perror("client,bufferevent_socket_connect");
        bufferevent_free(bufevin);
        bufferevent_free(bufevout);
        return;
    }

//    bufferevent_setcb(bufevout, read_cb_from_server, NULL, error_cb, bufevin);
    bufferevent_setcb(bufevin, read_cb_from_local, NULL, error_cb_from_local, bufevout);

    bufferevent_enable(bufevout, EV_READ|EV_WRITE);
    bufferevent_enable(bufevin, EV_READ|EV_WRITE);

}

void error_cb(struct bufferevent *bev, short error, void *ctx) {
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
//        fprintf(stderr, "connection has been closed!\n");
        perror("error_call_back");
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
//        fprintf(stderr, "error!\n");
        perror("error_call_back");
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        /* ... */
//        fprintf(stderr, "timeout!");
        perror("error_call_back");
    }
    bufferevent_free(bev);
}

void error_cb_from_local(struct bufferevent *bev, short what, void *ctx) {
    struct bufferevent *data = ctx;

    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        perror("local error");
        if (data) {
            if (evbuffer_get_length(bufferevent_get_output(data))) {
                bufferevent_setcb(bev, NULL, close_on_finish_write_cb, error_cb_from_local, data);
                bufferevent_disable(data, EV_READ);
            } else {
                bufferevent_free(bev);
            }
        }
    }
}

void read_cb_from_local(struct bufferevent *bev, void *ctx) {
    struct bufferevent *tar = ctx;

    byte temp[4096];
    bzero(&temp, sizeof(temp));
    size_t n;

    for (; ;) {
        n = bufferevent_read(bev, temp, sizeof(temp));

        // debug: show the message from browser!
        printf("message from browser: %d, %s\n", n, temp);

        if (n <= 0) {
            break; /*no more data*/
        }
        bufferevent_write_encode(tar, temp, sizeof(temp));
    }
}

void read_cb_from_server(struct bufferevent *bev, void *ctx){
    struct bufferevent *tar = ctx;

    char temp[4096];
    bzero(&temp, sizeof(temp));
    size_t n;

    for (; ;) {
        n = bufferevent_read_decode(bev, temp, sizeof(temp));
        if (n <= 0) {
            break;
        }
        bufferevent_write(tar, temp, sizeof(temp));
    }
}

void close_on_finish_write_cb(struct bufferevent *bev, void *ctx) {
    struct bufferevent *data = ctx;

    if (ctx && evbuffer_get_length(bufferevent_get_output(ctx)) == 0) {
        bufferevent_free(bev);
    }
}