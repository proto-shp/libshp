#ifndef CSOCKET_H
#define CSOCKET_H

#include "error.h"


typedef int csocket;
typedef enum csocket_type {
	TCP,
	UDP
} csocket_type;

typedef struct csocket_context {
	csocket_type sock_type;

	short port;
	char ip[17]; // dot seperated numbers

	char *buffer;
	int buffer_length;

	char *response;
	int response_length;
} csocket_context;

typedef shp_error (* csocket_handler) (csocket_context *);

shp_error csocket_close(const csocket sock);
shp_error csocket_udp_init(csocket* sock, const char *addr, unsigned int port);
shp_error csocket_tcp_init(csocket *sock, const char *addr, unsigned int port);

shp_error csocket_udp_listen(csocket sock, csocket_handler handler);
shp_error csocket_tcp_listen(csocket sock, csocket_handler handler);

shp_error csocket_tcp_send_to(csocket sock, short port, char *ip, char *buffer, int length);
shp_error csocket_udp_send_to(csocket sock, short port, char *ip, char *buffer, int length);
shp_error csocket_tcp_send(csocket sock, char *buffer, int length);

#endif
