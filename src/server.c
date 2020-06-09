#include <pthread.h>
#include <stdio.h>

#include "libshp/error.h"
#include "libshp/csocket.h"
#include "libshp/server.h"

// I am so sorry for this programming gods
csocket tcp_client;
csocket udp_client;
csocket tcp_server;
csocket udp_server;

void some_message_handler_from_jonas(csocket_context *c) {
	char *hello = "hello";
	printf("%u\n", c->sock_type);
    csocket_tcp_send_to(tcp_client, c->port, c->ip, hello, 5);
}

void *event_handler(void *arg) { while(1) { } return NULL; }

void *tcp_server_handler(void *arg) { 
	shp_error error = csocket_tcp_listen(tcp_server, *(csocket_handler)arg);
	if (error != 0) { /*handle the error*/ } 
	return NULL;
}

void *udp_server_handler(void *arg) { 
	shp_error error = csocket_udp_listen(udp_server, *(csocket_handler)arg);
	if (error != 0) { /*handle the error*/ } 
	return NULL;
}

shp_error server_start(server_config *config) {
	csocket_tcp_init(&tcp_client, config->ip, config->client_port);
    csocket_udp_init(&udp_client, config->ip, config->client_port);
    csocket_tcp_init(&tcp_server, config->ip, config->server_port);
    csocket_udp_init(&udp_server, config->ip, config->server_port);
	
	pthread_t tcp_server_thread, udp_server_thread, event_thread;

	// the handler will not be passed in in the future
	pthread_create(&tcp_server_thread, NULL, tcp_server_handler, some_message_handler_from_jonas);
	pthread_create(&udp_server_thread, NULL, udp_server_handler, some_message_handler_from_jonas);
	pthread_create(&event_thread, NULL, event_handler, NULL);

	pthread_join(tcp_server_thread, NULL);
	pthread_join(udp_server_thread, NULL);
	pthread_join(event_thread, NULL);
	return 0;
}