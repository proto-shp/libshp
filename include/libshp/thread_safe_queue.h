#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <stdint.h>
#include "libshp/message.h"

#include <semaphore.h>

sem_t *empty_count;
sem_t *full_count;

typedef struct{
	Message_Header *queue;
	uint32_t length;
	uint32_t count;
	uint32_t head;
	uint32_t tail;
} Message_Queue;

Message_Queue make_message_queue(uint32_t queue_length);
int pull(Message_Queue *message_queue, Message_Header *message);
int push(Message_Queue *message_queue, Message_Header message);
#endif
