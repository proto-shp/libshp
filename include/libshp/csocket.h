#ifndef CSOCKET_H
#define CSOCKET_H

#include "error.h"

typedef enum csocket_type {
	TCP,
	UDP
} csocket_type;

typedef int csocket;

shp_error csocket_close(const csocket *sock);
shp_error csocket_udp_init(csocket* sock, const char *addr, unsigned int port);
shp_error csocket_tcp_init(csocket *sock, const char *addr, unsigned int port, int is_server);

#endif