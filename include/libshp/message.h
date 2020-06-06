#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define HEADER_DEFAULT_LENGTH 12;
#define HEADER_MAGIC_NUMBER 0xcafe;

#define DEFAULT_SPACE_FOR_PAYLOAD_ARGS 512
#define DEFAULT_PAYLOAD_LENGTH 8

typedef struct Message_Payload Message_Payload;
struct Message_Payload{
	uint32_t length;
	uint32_t function_id;

	int allocated_space;
	void *arguments;

	Message_Payload *next;
};

typedef struct{
	uint32_t content_length;
	uint16_t magic_number;
	uint16_t flags;
	uint32_t message_id;

	//First item in linked list
	Message_Payload *payload;
} Message_Header;

Message_Header make_message();
Message_Payload make_message_payload();

uint32_t serialize_message(Message_Header *message, void **result);

//Please do not free raw until this message is done being used
Message_Header deserialize_message(void *raw);

void add_message_payload(Message_Header *message, Message_Payload *payload);

// Functions for writing arguments to a payload
// TODO Add 64 bit numbers
void add_u32(Message_Payload *payload, uint32_t a);
void add_s32(Message_Payload *payload, int32_t a);

void add_u16(Message_Payload *payload, uint16_t a);
void add_s16(Message_Payload *payload, int16_t a);

void add_u8(Message_Payload *payload, uint16_t a);
void add_s8(Message_Payload *payload, uint16_t a);


#endif
