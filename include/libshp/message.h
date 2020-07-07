#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define HEADER_DEFAULT_LENGTH 12;
#define HEADER_MAGIC_NUMBER 0xcafe;

#define DEFAULT_SPACE_FOR_PAYLOAD_ARGS 512
#define DEFAULT_PAYLOAD_LENGTH 8
void init_semaphores();

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
void add_u32_array(Message_Payload *payload, uint32_t *values, uint32_t count);
void add_s32_array(Message_Payload *payload, int32_t *values, uint32_t count);

void add_u32(Message_Payload *payload, uint32_t a);
void add_s32(Message_Payload *payload, int32_t a);

void add_u16_array(Message_Payload *payload, uint16_t *values, uint32_t count);
void add_s16_array(Message_Payload *payload, int16_t *values, uint32_t count);

void add_u16(Message_Payload *payload, uint16_t a);
void add_s16(Message_Payload *payload, int16_t a);

void add_u8_array(Message_Payload *payload, uint8_t *values, uint32_t count);
void add_s8_array(Message_Payload *payload, int8_t *values, uint32_t count);

void add_u8(Message_Payload *payload, uint8_t a);
void add_s8(Message_Payload *payload, int8_t a);

uint32_t get_u32_array(void **data, uint32_t **result);
uint32_t get_s32_array(void **data, int32_t **result);

uint32_t get_u32(void **data);
int32_t get_s32(void **data);

uint32_t get_u16_array(void **data, uint16_t **result);
uint32_t get_s16_array(void **data, int16_t **result);

uint16_t get_u16(void **data);
int16_t get_s16(void **data);

uint32_t get_u8_array(void **data, uint8_t **result);
uint32_t get_s8_array(void **data, int8_t **result);

uint8_t get_u8(void **data);
int8_t get_s8(void **data);

#endif
