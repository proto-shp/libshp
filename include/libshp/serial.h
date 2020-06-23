#ifndef SERIAL_H
#define SERIAL_H

#include "libshp/message.h"

#define U8 0
#define S8 1
#define U16 2
#define S16 3
#define U32 4
#define S32 5
#define STRING 6

#define ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <='Z'))

typedef struct{
	int type;
	char is_array;
}Type;

int write_message_payload(const char *format, Message_Payload *message, ...);
int read_message_payload(const char *format, Message_Payload *message, ...);

#endif
