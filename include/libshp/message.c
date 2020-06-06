#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Temporary
#include <stdio.h>

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
	if(payload->length - 8 + count > payload->allocated_space){
		payload->arguments = realloc(payload->arguments, payload->allocated_space * 2);
		payload->allocated_space *= 2;
	}
	memcpy(payload->arguments + payload->length - DEFAULT_PAYLOAD_LENGTH, bytes, count);
	payload->length += count;
}

uint32_t serialize_message(Message_Header *message, void **result){
	void *r = malloc(message->content_length);
	//TODO check this worked

	uint32_t written = 0;
	((uint32_t *)(r+written))[0] = htonl(message->content_length);
	written += 4;
	
	((uint16_t *)(r+written))[0] = htons(message->magic_number);
	written += 2;
	
	((uint16_t *)(r+written))[0] = htons(message->flags);
	written += 2;
	
	((uint32_t *)(r+written))[0] = htonl(message->message_id);
	written += 4;

	Message_Payload *p = message->payload;

	while(p != NULL){
		((uint32_t *)(r+written))[0] = ntohl(p->length);
		written += 4;
	
		((uint32_t *)(r+written))[0] = ntohl(p->function_id);
		written += 4;
		
		memcpy(r+written, p->arguments, p->length - DEFAULT_PAYLOAD_LENGTH);
		written += p->length - DEFAULT_PAYLOAD_LENGTH;

		p = p->next;
	}
	
	assert(written == message->content_length);

	*result = r;
	return message->content_length;
}

Message_Header deserialize_message(void *raw){
	Message_Header header;
	uint32_t read = 0;
	header.content_length = ntohl(((uint32_t *)(raw))[0]);
	read += 4;

	header.magic_number = ntohs(((uint16_t *)(raw+read))[0]);
	read += 2;

	header.flags = ntohs(((uint16_t *)(raw+read))[0]);
	read += 2;

	header.message_id = ntohl(((uint32_t *)(raw+read))[0]);
	read += 4;

	Message_Payload *current = NULL;

	while(read < header.content_length){
		uint32_t length = ntohl(((uint32_t *)(raw+read))[0]);
		read += 4;

		uint32_t function_id = ntohl(((uint32_t *)(raw+read))[0]);
		read += 4;

		//TODO This is over allocating, don't want to complicate further yet
		void *space = malloc(sizeof(Message_Payload) + length);
		Message_Payload *payload = (Message_Payload *)space;
		payload->length = length;
		payload->function_id = function_id;

		payload->arguments = space + sizeof(Message_Payload);
		payload->allocated_space = length - DEFAULT_PAYLOAD_LENGTH;
		memcpy(payload->arguments, raw+read, length - DEFAULT_PAYLOAD_LENGTH);


		read += length - DEFAULT_PAYLOAD_LENGTH;
		
		if(current == NULL){
			current = payload;
			header.payload = current;
		}else{
			current->next = payload;
			current = payload;
		}
	}

	assert(read == header.content_length);

	return header;
}

void add_u32(Message_Payload *payload, uint32_t a){
	uint32_t write = htonl(a);
	write_bytes(payload, (uint8_t *)&write, 4);
}

void add_s32(Message_Payload *payload, int32_t a){
	uint32_t write = htonl((uint32_t) a);
	write_bytes(payload, (uint8_t *)&write, 4);
}

uint32_t get_u32(void **data){
	void *d = *data;
	uint32_t result = ntohl(((uint32_t *)d)[0]);
	*data = *data+4;

	return result;
}

int32_t get_s32(void **data){
	void *d = *data;
	int32_t result = ntohl(((uint32_t *)d)[0]);
	*data = *data+4;

	return result;
}

int main(){
	Message_Header message = make_message();
	Message_Payload payload = make_message_payload();
	add_s32(&payload, 1234);
	add_u32(&payload, 1997);

	add_message_payload(&message, &payload);

	void *result = NULL;
	uint32_t l = serialize_message(&message, &result);

	for(int i = 0; i< l; i++){
		printf("%x ", ((uint8_t *)result)[i]);
		if((i+1)%8 == 0){
			printf("\n");
		}
	}

	Message_Header back = deserialize_message(result);
	printf("Content_length: %d, magic_number = %x, flags = %d, message_id = %d\n", back.content_length, back.magic_number, back.flags, back.message_id);

	void *args = back.payload->arguments;
	int32_t first = get_s32(&args);
	uint32_t second = get_u32(&args);

	printf("first = %d, second = %d\n", first, second);

	args = back.payload->arguments;
	first = get_s32(&args);
	second = get_u32(&args);

	printf("first = %d, second = %d\n", first, second);



	return 0;
}
