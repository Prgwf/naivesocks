//
// Created by wfxzz on 3/27/18.
//

#include "server.h"

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

  printf("%s", config.cipher);

  struct event_base *base;
  base = event_base_new();
  if (base == NULL) {
    fprintf(stderr, "base new error\n");
    return 1;
  }

  struct sockaddr_in sin;
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0); /* 0.0.0.0 */
  sin.sin_port = htons(8998);

  struct evconnlistener *listener;
  listener = evconnlistener_new_bind(base, do_accept, base,
				     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
				     (struct sockaddr *)&sin, sizeof(sin));

  if (!listener) {
    fprintf(stderr, "new listener error\n");
    event_base_free(base);
    return 1;
  }

  // todo evconnlistener_set_error_cb();

  event_base_dispatch(base);

  evconnlistener_free(listener);
  event_base_free(base);

  return 0;
}

void do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int length, void *ctx) {
  printf("server: accept from local\n");
  struct event_base *base = ctx;
  struct INFO *info;
  struct bufferevent *bufevin;

  bufevin = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
  if (bufevin == NULL) {
    fprintf(stderr, "new bufferevent error\n");
    return;
  }

  info = malloc(sizeof(struct INFO));
  if (info == NULL) {
    bufferevent_free(bufevin);
    return;
  }

  info->mode = 0;
  info->bev = NULL;

  bufferevent_setcb(bufevin, read_cb, NULL, read_error_cb, info);
  bufferevent_enable(bufevin, EV_READ|EV_WRITE);
  printf("accept successfully\n");
}

void read_cb(struct bufferevent *bev, void *ctx) {
  struct INFO *info = ctx;

  switch (info->mode) {
  case SOCK5_HELLO:
    printf("read_cb handle HELLO\n");
    handleHello(bev, info);
    break;
  case SOCK5_CONNECT:
    printf("read_cb handle CONNECT\n");
    handleConnect(bev, info);
    break;
  case SOCK5_REQUEST:
    printf("read_cb handle REQUEST\n");
    handleRequest(bev, info);
    break;
  default:
    fprintf(stderr, "read_cb error\n");
    return;
  }
}

void read_error_cb(struct bufferevent *bev, short what, void *ctx) {
    struct INFO *info = ctx;
    if (what & BEV_EVENT_ERROR) {
        fprintf(stderr, "read_cb error\n");
	free(info);
	bufferevent_free(bev);
    }
}


void write_error_cb(struct bufferevent *bev, short what, void *ctx) {
    struct bufferevent *data = ctx;

    if (what & BEV_EVENT_ERROR) {
        fprintf(stderr, "write error\n");
	if (data) {
	  bufferevent_free(data);
	}
	bufferevent_free(bev);
    }
}

void handleHello(struct bufferevent *bev, struct INFO *info) {
/*
    The client connects to the server, and sends a version
    identifier/method selection message:

    +----+----------+----------+
    |VER | NMETHODS | METHODS  |
    +----+----------+----------+
    | 1  |    1     | 1 to 255 |
    +----+----------+----------+

   The VER field is set to X'05' for this version of the protocol.  The
   NMETHODS field contains the number of method identifier octets that
   appear in the METHODS field.
*/

    if (info->mode != SOCK5_HELLO) {
      fprintf(stderr, "Not Hello Step\n");
      bufferevent_free(bev);
      return;
    }

    int n;
    byte temp[4096];

    n = bufferevent_read_decode(bev, temp, BUF_SIZE);
    if (n < 3 || temp[0] != 0x05) {
      fprintf(stderr, "read error\n");
      bufferevent_free(bev);
      free(info);
      return;
    }

/*
   The server selects from one of the methods given in METHODS, and
   sends a METHOD selection message:

                         +----+--------+
                         |VER | METHOD |
                         +----+--------+
                         | 1  |   1    |
                         +----+--------+

          o  X'00' NO AUTHENTICATION REQUIRED
          o  X'01' GSSAPI
          o  X'02' USERNAME/PASSWORD
          o  X'03' to X'7F' IANA ASSIGNED
          o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
          o  X'FF' NO ACCEPTABLE METHODS


*/
    temp[0] = 0x05;
    temp[1] = 0x00; // NO AUTHENTICATION REQUIRED

    bufferevent_write_encode(bev, temp, 2);
    info->mode++;

    printf("server: sock5 hello!\n");
}

void handleConnect(struct bufferevent *bev, struct INFO *info) {
  if (info->mode != SOCK5_CONNECT) {
    fprintf(stderr, "Not Connect Step!\n");
    bufferevent_free(bev);
    free(info);
    return;
  }


/*
  The SOCKS request is formed as follows:

  +----+-----+-------+------+----------+----------+
  |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
  +----+-----+-------+------+----------+----------+
  | 1  |  1  | X'00' |  1   | Variable |    2     |
  +----+-----+-------+------+----------+----------+

  Where:

  o  VER    protocol version: X'05'
  o  CMD
  o  CONNECT X'01'
  o  BIND X'02'
  o  UDP ASSOCIATE X'03'
  o  RSV    RESERVED
  o  ATYP   address type of following address
  o  IP V4 address: X'01'
  o  DOMAINNAME: X'03'
  o  IP V6 address: X'04'
  o  DST.ADDR       desired destination address
  o  DST.PORT desired destination port in network octet
  order

*/

  int n;
  byte temp[BUF_SIZE];

  n = bufferevent_read_decode(bev, temp, BUF_SIZE);
  if (n < 7) {
    fprintf(stderr, "read error\n");
    bufferevent_free(bev);
    free(info);
    return;
  }

  if (temp[0] != 0x05 || temp[1] != 0x01) {
    fprintf(stderr, "CMD error\n");
    bufferevent_free(bev);
    free(info);
    return;
  }

  struct event_base *base = bufferevent_get_base(bev);
  
  if (base == NULL) {
    fprintf(stderr, "base error\n");
    bufferevent_free(bev);
    free(info);
    return;
  }

  struct sockaddr_in sin;
  socklen_t slen;

  char domain[1024], port[16];
  struct addrinfo hints;
  struct addrinfo *res;

  bzero(&sin, sizeof(sin));
  switch(temp[3]) {
  case 0x01:
    /* IPV4 ADDRESS*/
    sin.sin_family = AF_INET;
    memcpy(&(sin.sin_addr), temp+4, 4);
    memcpy(&(sin.sin_port), temp+n-2, 2);
    slen = sizeof(struct sockaddr_in);
    break;

  case 0x03:
    /* DOMAIN NAME*/
  {
    uint8_t port1, port2;
    bzero(&domain, sizeof(domain));
    memcpy(domain, temp+5, n-7);

    bzero(&port, sizeof(port));
    memcpy(port, temp+n-2, 2);
    port1 = port[0];
    port2 = port[1];
    sprintf(port, "%d", port1*256+port2); 

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;
    
    if (evutil_getaddrinfo(domain, port, &hints, &res) < 0) {
      fprintf(stderr, "get addrinfo error\n");
      bufferevent_free(bev);
      if (info)
	free(info); 
      return;
    }

    memcpy(&sin, res->ai_addr, res->ai_addrlen); 
    slen = res->ai_addrlen;
    evutil_freeaddrinfo(res);
    break;
  }
  case 0x04:
    /* IPV6 ADDRESS*/
    // todo
    break;

  default:
    fprintf(stderr, "address error\n");
    bufferevent_free(bev);
    free(info);
    return;
  }

  struct bufferevent *bev_new;

  bev_new = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  if (bev_new == NULL) {
    fprintf(stderr, "bufferevent socket new error\n");
    bufferevent_free(bev);
    free(info);
    return;
  }

  if (bufferevent_socket_connect(bev_new, (struct sockaddr *)&sin, slen) < 0) {
    bufferevent_free(bev);
    bufferevent_free(bev_new);
    free(info);
    return;
  }

  bufferevent_setcb(bev_new, NULL, NULL, write_error_cb, bev);
  bufferevent_enable(bev_new, EV_READ|EV_WRITE);

  info->mode += 1;
  info->bev = bev_new;

  printf("server: sock5 connection!\n");

}

void handleRequest(struct bufferevent *bev, struct INFO *info) {
    int n;
    byte *temp[BUF_SIZE];

    n = bufferevent_read_decode(bev, temp, BUF_SIZE);
    if (n != 0) {
        bufferevent_write(info->bev, temp, n);
    }

    printf("server: sock5 request!\n");
}
