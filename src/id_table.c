#include <stdlib.h>
#include <stdio.h>

#include "libshp/id_table.h"

int make_id_table(ID_Table *table, int wanted_capacity){
	table->table = (ID_Table_Entry *)malloc(sizeof(ID_Table_Entry) * wanted_capacity);
	if(!table->table){
		return 0;
	}

	table->capacity = wanted_capacity;
	table->length_used = 0;

	return wanted_capacity;
}


int compare_id_table_entries(const void *a, const void *b){
	return ((ID_Table_Entry *)(a))->id - ((ID_Table_Entry *)(b))->id;
}

void sort_table(ID_Table *table){
	qsort(table->table, table->length_used, sizeof(ID_Table_Entry), compare_id_table_entries);
}

int add_device(ID_Table *table, uint32_t id, uint32_t ip){
	if(table->length_used == table->capacity){
		return 0;
	}
	
	ID_Table_Entry entry;
	entry.id = id;
	entry.ip = ip;

	table->table[table->length_used++] = entry;

	sort_table(table);

	return 1;
}


uint32_t lookup_ip(ID_Table *table, uint32_t id){
	int left = 0;
	int right = table->length_used - 1;

	if(table->length_used == 0){
		return 0;
	}

//	if(id == table->table[0].id){
//		return table->table[0].ip;
//	}

	while(1){
	
		if(left > right){
			break;
		}

		int middle = left + (right-left)/2;

		uint32_t this_id = table->table[middle].id;

		if(this_id == id){
			return table->table[middle].ip;
		}else if(this_id > id){
			right = middle - 1;
		}else{
			left = middle + 1;
		}		
	}
	return 0;
}

void print_table(ID_Table *table){
	printf("Capacity = %d, Length Used = %d\n", table->capacity, table->length_used);
	for(int i = 0; i < table->length_used; i++){
		ID_Table_Entry entry = table->table[i];
		printf("IP = %d, IP = %d\n", entry.id, entry.ip);
	}
}

/* Leaving this here because I will use it for adding tests later
int main(){
	ID_Table table;
	if(make_id_table(&table, 10) == 0){
		printf("Failed somehow to init the table\n");
		return 1;
	}
	
	print_table(&table);
	add_device(&table, 42, 101010);
	add_device(&table, 12, 111111);
	add_device(&table, 20, 100001);
	add_device(&table, 43, 110011);
	print_table(&table);

	printf("%d\n", lookup_ip(&table, 42));
	printf("%d\n", lookup_ip(&table, 12));
	printf("%d\n", lookup_ip(&table, 20));
	printf("%d\n", lookup_ip(&table, 43));
	printf("%d\n", lookup_ip(&table, 666));
	printf("%d\n", lookup_ip(&table, 2));
	printf("%d\n", lookup_ip(&table, 13));

	return 0;
}
*/
