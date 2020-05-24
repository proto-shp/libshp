#include "./unit.h"
#include <stdio.h>

#include "libshp/csocket.h"
#include "libshp/id_table.h"

const char *test1() {
    csocket sock;
    csocket_udp_init(&sock, "", 5400);
	pass();
}

const char *test2() {
	ID_Table table;
	unit_assert("Failed to initialize ID_Table\n", make_id_table(&table, 10) == 10);
	
	add_device(&table, 42, 101010);
	add_device(&table, 12, 111111);
	add_device(&table, 20, 100001);
	add_device(&table, 43, 110011);

	unit_assert("Expected 101010\n", lookup_ip(&table, 42) == 101010);
	unit_assert("Expected 111111\n", lookup_ip(&table, 12) == 111111);
	unit_assert("Expected 100001\n", lookup_ip(&table, 20) == 100001);
	unit_assert("Expected 110011\n", lookup_ip(&table, 43) == 110011);
	unit_assert("Expected 0\n",      lookup_ip(&table, 666) == 0);
	unit_assert("Expected 0\n",      lookup_ip(&table, 2) == 0);
	unit_assert("Expected 0\n",      lookup_ip(&table, 13) == 0);

	pass();
}

int main() {

	TESTS (
		test1,
		test2
	);

    return 0;
}
