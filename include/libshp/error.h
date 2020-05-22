#ifndef ERROR_H
#define ERROR_H

#define DEBUG 0

#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

typedef enum {
    SUCCESS = 0
} shp_error;

const char *shp_strerror(shp_error code);

#endif