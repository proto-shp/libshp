#ifndef ID_TABLE_H
#define ID_TABLE_H

#include <stdint.h>

typedef struct{
	uint32_t id;
	uint32_t ip;
} ID_Table_Entry;

typedef struct{
	ID_Table_Entry *table;
	int capacity;
	int length_used;
} ID_Table;

//Returns the size of the table actually allocated
int make_id_table(ID_Table *table, int wanted_capacity);

int add_device(ID_Table *table, uint32_t id, uint32_t ip);

uint32_t lookup_ip(ID_Table *table, uint32_t id);

#endif
