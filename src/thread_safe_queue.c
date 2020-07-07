#include "libshp/thread_safe_queue.h"

#include <stdlib.h>
#include <stdio.h>

Message_Queue make_message_queue(uint32_t queue_length){
	Message_Queue message_queue;

	message_queue.queue = malloc(sizeof(Message_Header) * queue_length);
	if(message_queue.queue == NULL){
		fprintf(stderr, "Failed to allocate space for the message queue\n");
		exit(-1);
	}
	message_queue.length = queue_length;
	message_queue.head = 0;
	message_queue.tail = 0;
	message_queue.count = 0;

	return message_queue;
}

//If queue is empty return 0, else return 1 if data was read from queue
int pull(Message_Queue* message_queue, Message_Header *message){
	//The queue is empty
	if(message_queue->count == 0) return 0;
	//TODO probably copy the contents so we don't have to worry about ownership
	*message = message_queue->queue[message_queue->tail];
	message_queue->tail = (message_queue->tail+1)%message_queue->length;
	message_queue->count--;
	return 1;
}

//If queue has space push and return 1, otherwise return 0 and try again later?
int push(Message_Queue *message_queue, Message_Header message){
	//Queue is full, can't push
	if(message_queue->count == message_queue->length){
		return 0;
	}
	message_queue->queue[message_queue->head] = message;
	message_queue->head = (message_queue->head+1)%message_queue->length;
	message_queue->count++;
	return 1;
}
