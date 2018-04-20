//
// Created by wfxzz on 3/27/18.
//

#include "client.h"

void usage() {
  printf("usage:\n");
  printf("-c config file path\n");
  printf("-d daemon (todo)\n");
}

int main(int argc, char **argv) {
  int opt;
  char config_file_pos[128];
  bzero(&config_file_pos, sizeof(config_file_pos));

  while ((opt=getopt(argc,argv, "c:")) != -1) {
    switch(opt) {
    case 'c':
      sprintf(config_file_pos, "%s", optarg);
      break;
    case 'd':
      // todo daemon
      break;
	
    default:
      usage();
      break;
    }
  }

  struct js config = easy_paser(config_file_pos);
  size_t length = strlen(config.cipher);
  byte *map = cipher2map((byte *)config.cipher, length);
  get_encode_decode_map(map);

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

  struct sockaddr_in remote_addr;
  bzero(&remote_addr, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(atoi(config.server_port));
//    inet_aton(config.server_address, &remote_addr.sin_addr);
  remote_addr.sin_addr = local_addr.sin_addr; /* local test */


  struct Reomte *remote_info;
  remote_info = malloc(sizeof(remote_info));
  if (remote_info == NULL) {
    perror("new remote_info");
    return 1;
  }
  remote_info->sin = remote_addr;
  remote_info->base = base;

  listener = evconnlistener_new_bind(base, do_accept, remote_info,
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

  return 0;
}


void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx) {
    // debug by printf
  printf("server: accept from browser\n");

  struct Reomte *info = ctx;
  struct event_base *base = evconnlistener_get_base(listener);
  struct sockaddr_in sin = info->sin;

  struct bufferevent *bufevin;
  bufevin = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  if (bufevin == NULL) {
    perror("bufevin");
    return;
  }

  struct bufferevent *bufevout;
  bufevout = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE); // here: fd = -1 means set later...
  if (bufevout == NULL) {
    perror("bufevout");
    return;
  }

  if (bufferevent_socket_connect(bufevout, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("client --> server, bufferevent_socket_connect");
    bufferevent_free(bufevin);
    bufferevent_free(bufevout);
    return;
  }

  bufferevent_setcb(bufevout, read_cb_from_server, NULL, error_cb_from_server, bufevin);
  bufferevent_setcb(bufevin, read_cb_from_local, NULL, error_cb_from_local, bufevout);

  bufferevent_enable(bufevout, EV_READ|EV_WRITE);
  bufferevent_enable(bufevin, EV_READ|EV_WRITE);
}

void error_cb_from_server(struct bufferevent *bev, short what, void *ctx) {
  if (what & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
    if (what & BEV_EVENT_ERROR) {
      perror("error_cb from_server");
    }
    
    bufferevent_free(bev);
  } 
  
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
    byte temp[BUF_SIZE];
    bzero(&temp, sizeof(temp));
    size_t n;
    

    n = bufferevent_read(bev, temp, BUF_SIZE);

    printf("message from browser: %ld\n", n);

    if (n <= 0) {
      return;
    }

    printf("now local encode ctx and write to buffer\n");
    
    bufferevent_write_encode(tar, temp, n);
}

void read_cb_from_server(struct bufferevent *bev, void *ctx){
  struct bufferevent *tar = ctx;

  byte temp[BUF_SIZE];
  bzero(&temp, sizeof(temp));
  size_t n;

  for (;;) {
    n = bufferevent_read_decode(bev, temp, BUF_SIZE);
    if (n <= 0) {
      break;
    }
    bufferevent_write(tar, temp, n);
  }
}

void close_on_finish_write_cb(struct bufferevent *bev, void *ctx) {
    struct bufferevent *data = ctx;

    if (ctx && evbuffer_get_length(bufferevent_get_output(ctx)) == 0) {
        bufferevent_free(bev);
    }
}
