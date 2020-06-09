#include "./unit.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "libshp/csocket.h"
#include "libshp/server.h"

csocket tcp_client;
csocket udp_client;

csocket tcp_server;
csocket udp_server;

shp_error handler(csocket_context *c) {
    char *hello = "hello";
    csocket_tcp_send_to(tcp_client, c->port, c->ip, hello, 5);
    return 0;
}

const char *test1() {
    csocket sock;
    csocket_udp_listen(udp_server, handler);
	pass();
}

int main() {
    server_config config = { "192.168.31.78", 5200, 5201 };
    server_start(&config);

	TESTS (
		test1
	);

    return 0;
}