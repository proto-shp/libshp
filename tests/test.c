#include "./unit.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "libshp/csocket.h"

csocket tcp_client;

shp_error handler(csocket_context *c) {
    printf("%i\n", ntohl(*(uint32_t *)c->buffer));
    printf("here %s %i\n", c->buffer + 4, c->buffer_length);

    char *hello = "hello";
    printf("%s     %i\n", c->ip, c->port);
    csocket_tcp_send_to(tcp_client, c->port, c->ip, hello, 5);
    return 0;
}

const char *test1() {
    csocket sock;

    csocket_tcp_init(&tcp_client, "", 5200);
    // csocket_tcp_listen(sock, handler);
    csocket_udp_init(&sock, "", 5200);
    csocket_udp_listen(sock, handler);
	pass();
}

int main() {

	TESTS (
		test1
	);

    return 0;
}