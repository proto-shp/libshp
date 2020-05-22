#include <string.h>
#include <errno.h>

#include "libshp/csocket.h"

#ifdef _WIN32
	// can put windows trash here
#else
	#include <unistd.h>
	#include <stdio.h> 
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include<pthread.h>
#endif

shp_error csocket_init(csocket* sock, const char *addr, unsigned int port, csocket_type type) {
    int status = 0;

    struct sockaddr_in addr_info = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(port),
    };

    *sock = socket(AF_INET, type == TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (*sock == 0) return 1;

    int reuse_addr = 1;
    status = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    if (status != 0) return 1;

    status = bind(*sock, (struct sockaddr *)&addr_info, sizeof(addr_info));
    if (status < 0) return 1;

    return 0;
}

shp_error csocket_udp_init(csocket* sock, const char *addr, unsigned int port) {
    int status = csocket_init(sock, addr, port, UDP);
    if (status != 0) return status;

    int broadcast = 1;
    status = setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    if (status != 0) return 1;

    return 0;
}

shp_error csocket_tcp_init(csocket *sock, const char *addr, unsigned int port, int is_server) {
    int status = csocket_init(sock, addr, port, TCP);
    if (status != 0) return status;

    if (is_server) {
        status = listen(*sock, 20);
        if (status < 0) return 1;
    }

    return 0;
}

shp_error csocket_close(const csocket *sock) {
#ifdef _WIN32
    closesocket(*sock);
    WSACleanup();
#else
    close(*sock);
#endif
    return 0;
}