#include "error.h"

typedef struct server_config {
	char ip[17];
	short server_port;
	short client_port;
} server_config;

shp_error server_start(server_config *config);
