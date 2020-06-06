#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(__linux__) || defined(__APPLE__)
	#include <arpa/inet.h>
#elif defined(_WIN32)
	#include <winsock.h>
#endif

Message_Header make_message(){
	Message_Header header;
	header.content_length = HEADER_DEFAULT_LENGTH;
	header.magic_number = HEADER_MAGIC_NUMBER;
	header.flags = 0;
	header.message_id = 0; //Set this to zero initially

	header.payload = NULL;

	return header;
}

Message_Payload make_message_payload(){
	Message_Payload payload;
	payload.length = DEFAULT_PAYLOAD_LENGTH;
	payload.function_id = 0;

	payload.allocated_space = DEFAULT_SPACE_FOR_PAYLOAD_ARGS;
	payload.arguments = malloc(DEFAULT_SPACE_FOR_PAYLOAD_ARGS);

	payload.next = NULL;

	return payload;
}

Message_Payload *get_tail(Message_Payload *head){
	if(head == NULL){
		return NULL;
	}
	while(head->next != NULL){
		head = head->next;
	}
	return head;
}

void add_message_payload(Message_Header *message, Message_Payload *payload){
	if(message->payload == NULL){
		message->payload = payload;
	}else{
		//This should never return NULL because we checked for that case above
		get_tail(message->payload)->next = payload;
	}
	message->content_length += payload->length;
}

void write_bytes(Message_Payload *payload, const uint8_t *bytes, size_t count){
	if(payload->length - 64 + count > payload->allocated_space){
		payload->arguments = realloc(payload->arguments, payload->allocated_space * 2);
		payload->allocated_space *= 2;
	}
	memcpy(payload->arguments + payload->length - 64, bytes, count);
	payload->length += count;
}

uint32_t find_payload_length(Message_Payload *payload){
	uint32_t result = 0;

	while(payload != NULL){
		result += payload->length;
		payload = payload->next;
	}

	return result;
}

uint32_t serialize_message(Message_Header *message, void **result){
	void *r = malloc(message->content_length);
	//TODO check this worked

	uint32_t written = 0;
	((uint32_t *)(r+written))[0] = ntohl(message->content_length);
	written += 4;
	
	((uint16_t *)(r+written))[0] = ntohs(message->magic_number);
	written += 2;
	
	((uint16_t *)(r+written))[0] = ntohs(message->flags);
	written += 2;
	
	((uint32_t *)(r+written))[0] = ntohl(message->message_id);
	written += 4;

	Message_Payload *p = message->payload;

	while(p != NULL){
		((uint32_t *)(r+written))[0] = ntohl(p->length);
		written += 4;
	
		((uint32_t *)(r+written))[0] = ntohl(p->function_id);
		written += 4;
		
		memcpy(r+written, p->arguments, p->length - 64);
		written += p->length - 64;

		p = p->next;
	}
	
	assert(written == message->content_length);

	*result = r;
	return message->content_length;
}

void add_u32(Message_Payload *payload, uint32_t a){
	uint32_t write = htonl(a);
	write_bytes(payload, (uint8_t *)&write, 4);
}

void add_s32(Message_Payload *payload, int32_t a){
	uint32_t write = htonl((uint32_t) a);
	write_bytes(payload, (uint8_t *)&write, 4);
}

int main(){
	Message_Header message = make_message();
	Message_Payload payload = make_message_payload();
	add_s32(&payload, 1234);

	add_message_payload(&message, &payload);

	return 0;
}
