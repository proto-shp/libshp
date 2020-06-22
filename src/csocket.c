#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "libshp/csocket.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
	#include <unistd.h>
	#include <stdio.h> 
	#include <sys/socket.h>
	#include <netinet/in.h>
    #include <arpa/inet.h>
#endif

//PLATFORM windows needs an init function to startup socket lib
#ifdef _WIN32
    shp_error init_sock_lib() {}
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

shp_error csocket_tcp_init(csocket *sock, const char *addr, unsigned int port) {
    int status = csocket_init(sock, addr, port, TCP);
    if (status != 0) return status;

    return 0;
}

// This function is only used to respond to an open client socket
// sock if an open client socket from a server with pre-existing connection
shp_error csocket_tcp_send(csocket sock, char *buffer, int length) {
    int bytes = write(sock, buffer, length);
    if (bytes < 1) {
        printf("Nothing was written to the socket\n");
    }

    return 0;
}

// opens a new client connection with the port and ip provided and sends the data at buffer
// sock is the client socket for sending outgoing data
shp_error csocket_tcp_send_to(csocket sock, short port, char *ip, char *buffer, int length) {
    struct sockaddr_in send_addr; 
    send_addr.sin_family = AF_INET; 
    send_addr.sin_port = htons(port); 
    int resp = inet_pton(AF_INET, ip, &(send_addr.sin_addr));
    if (resp == 0) {
        printf("IP is in an invalid format\n");
    }

    if (connect(sock, (struct sockaddr *)&send_addr, sizeof(send_addr)) != 0) { 
        printf("Could not connect with the server\n");
    }

    int bytes = write(sock, buffer, length);
    if (bytes < 1) {
        printf("Nothing was written to the socket\n");
    }

    return 0;
}

// send tcp buffer data to a machine with ip and port provided
shp_error csocket_udp_send_to(csocket sock, short port, char *ip, char *buffer, int length) {
    struct sockaddr_in recv_addr; 
    recv_addr.sin_family = AF_INET; 
    recv_addr.sin_port = htons(port); 
    int resp = inet_pton(AF_INET, ip, &(recv_addr.sin_addr));
    if (resp == 0) {
        printf("IP is in an invalid format\n");
    }

    int bytes = sendto(sock, buffer, length, 0, (struct sockaddr *) &recv_addr, sizeof(recv_addr));
    if (bytes < 1) {
        printf("Nothing was written to the socket\n");
    }

    return 0;
}

// listen to the TCP socket and copy all dat from the socket and send to handler.
// this function will never return.
shp_error csocket_tcp_listen(csocket sock, csocket_handler handler) {
    char *message;
    uint32_t message_size;
    struct sockaddr_in addr;
    csocket_context context;

    int status = listen(sock, 20);
    if (status < 0) return 1;
    
    socklen_t size = sizeof(struct sockaddr_in);
    while (1) {
        int client_sock = accept(sock, (struct sockaddr *)&addr, &size);
        if (client_sock < 0) {
            printf("Error Accepting Client\n");
        }

        uint32_t msg_len_big = 0;
        int bytes = read(client_sock, &msg_len_big, sizeof(uint32_t));
        message_size = ntohl(msg_len_big);

        message = (char *) calloc(message_size + 1, sizeof(char));
        if (message == NULL) {
            printf("Memory Allocation Error\n");
        }

        memcpy(message, &msg_len_big, sizeof(uint32_t));
        while (bytes < message_size) {
            int new_bytes = read(client_sock, message + bytes, message_size - bytes);
            if (new_bytes < 1) {
                printf("Error Reading The Socket\n");
            }

            bytes += new_bytes;
        }
        message[message_size] = '\0';
    
        // initialize request context
        inet_ntop(AF_INET, &(addr.sin_addr), context.ip, 16);
        context.port = ntohs(addr.sin_port);
        context.buffer = message;
        context.buffer_length = message_size;
        context.sock_type = TCP;

        handler(&context);
        free(message);
        csocket_close(client_sock);
    }

    return 0;
}

// launch a upd socket server, this function will never return
shp_error csocket_udp_listen(csocket sock, csocket_handler handler) {
    char message[1024];
    struct sockaddr_in addr;
    csocket_context context;

    while (1) {
        socklen_t size = sizeof(struct sockaddr_in);
        int bytes = recvfrom(sock, message, 1024, 0, (struct sockaddr *)&addr, &size);
        message[bytes] = '\n';
        inet_ntop(AF_INET, &(addr.sin_addr), context.ip, 16);

        // initialize request context
        context.port = ntohs(addr.sin_port);
        context.buffer = message;
        context.buffer_length = bytes;
        context.sock_type = UDP;
        
        shp_error err = handler(&context);
    }

    return 0;
}

shp_error csocket_close(const csocket sock) {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0;
}
