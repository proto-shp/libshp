#include "./unit.h"
#include <stdio.h>

#include "libshp/csocket.h"
#include "libshp/id_table.h"
#include "libshp/message.h"

const char *test1() {
    csocket sock;
    csocket_udp_init(&sock, "", 5400);
	pass();
}

const char *id_table() {
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

const char *message() {
	Message_Header message = make_message();
	message.flags = 0xabcd;
	message.message_id = 88;

	Message_Payload payload = make_message_payload();
	add_s32(&payload, 1234);
	add_u32(&payload, 1997);

	add_message_payload(&message, &payload);

	void *result = NULL;
	serialize_message(&message, &result);


	Message_Header back = deserialize_message(result);

	unit_assert("Expected content_length changed\n", back.content_length == message.content_length);
	unit_assert("Expected 0xcafe\n", back.magic_number == 0xcafe);
	unit_assert("Expected 0xabcd\n", back.flags == 0xabcd);
	unit_assert("Expected 88\n", back.message_id == 88);


	void *args = back.payload->arguments;
	int32_t first = get_s32(&args);
	uint32_t second = get_u32(&args);

	unit_assert("Expected 1234\n", first == 1234);
	unit_assert("Expected 1997\n", second == 1997);

	pass();
}

int main() {

	TESTS (
		test1,
		id_table,
		message
	);

    return 0;
}
