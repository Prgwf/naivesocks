//
// Created by wfxzz on 3/27/18.
//

#ifndef NAIVESOCKS_SERVER_H
#define NAIVESOCKS_SERVER_H

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

#define SOCK5_HELLO    0x00
#define SOCK5_CONNECT  0x01
#define SOCK5_REQUEST  0x02

/*




      +----+------+------+----------+----------+----------+
      |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
      +----+------+------+----------+----------+----------+
      | 2  |  1   |  1   | Variable |    2     | Variable |
      +----+------+------+----------+----------+----------+
*/


struct INFO {
    short mode;
    struct bufferevent *bev;
};

#endif //NAIVESOCKS_SERVER_H
