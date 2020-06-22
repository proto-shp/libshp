#include "./unit.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "libshp/csocket.h"
#include "libshp/id_table.h"
#include "libshp/message.h"
#include "libshp/server.h"

csocket tcp_client;
csocket udp_client;

csocket tcp_server;
csocket udp_server;

shp_error handler(csocket_context *c) {
    char *hello = "hello";
    csocket_tcp_send_to(tcp_client, c->port, c->ip, hello, 5);
    return 0;
}


const char *test1() {
    csocket sock;
    csocket_udp_listen(udp_server, handler);
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

	add_s16(&payload, -321);
	add_u16(&payload, 111);

	add_s8(&payload, -1);
	add_u8(&payload, 1);

	uint32_t a[3] = {1, 1, 1};
	int32_t b[3] = {-1, -1, -1};

	add_u32_array(&payload, a, 3);
	add_s32_array(&payload, b, 3);

	uint16_t c[2] = {2, 2};
	int16_t d[2] = {-2, -2};

	add_u16_array(&payload, c, 2);
	add_s16_array(&payload, d, 2);

	uint8_t e[1] = {1};
	int8_t f[1] = {-1};

	add_u8_array(&payload, e, 1);
	add_s8_array(&payload, f, 1);

	add_message_payload(&message, &payload);

	void *result = NULL;
	serialize_message(&message, &result);


	Message_Header back = deserialize_message(result);

	unit_assert("Expected content_length changed\n", back.content_length == message.content_length);
	unit_assert("Expected 0xcafe\n", back.magic_number == 0xcafe);
	unit_assert("Expected 0xabcd\n", back.flags == 0xabcd);
	unit_assert("Expected 88\n", back.message_id == 88);


	void *args = back.payload->arguments;
	int32_t s32 = get_s32(&args);
	uint32_t u32 = get_u32(&args);
	
	int16_t s16 = get_s16(&args);
	uint16_t u16 = get_u16(&args);

	int8_t s8 = get_s8(&args);
	uint8_t u8 = get_u8(&args);

	unit_assert("Expected 1234\n", s32 == 1234);
	unit_assert("Expected 1997\n", u32 == 1997);

	unit_assert("Expected 1234\n", s16 == -321);
	unit_assert("Expected 1997\n", u16 == 111);
	
	unit_assert("Expected 1234\n", s8 == -1);
	unit_assert("Expected 1997\n", u8 == 1);

	uint32_t *ra;
	int32_t *rb;
	uint16_t *rc;
	int16_t *rd;
	uint8_t *re;
	int8_t *rf;
	
	uint32_t ca = get_u32_array(&args, &ra);
	uint32_t cb = get_s32_array(&args, &rb);
	uint32_t cc = get_u16_array(&args, &rc);
	uint32_t cd = get_s16_array(&args, &rd);
	uint32_t ce = get_u8_array(&args, &re);
	uint32_t cf = get_s8_array(&args, &rf);

	unit_assert("Size should be 3\n", ca == 3);
	unit_assert("Size should be 3\n", cb == 3);
	unit_assert("Size should be 2\n", cc == 2);
	unit_assert("Size should be 2\n", cd == 2);
	unit_assert("Size should be 1\n", ce == 1);
	unit_assert("Size should be 1\n", cf == 1);
	
	unit_assert("Value should be 1\n", ra[0]==1);
	unit_assert("Value should be 1\n", ra[1]==1);
	unit_assert("Value should be 1\n", ra[2]==1);

	unit_assert("Value should be -1\n", rb[0]==-1);
	unit_assert("Value should be -1\n", rb[1]==-1);
	unit_assert("Value should be -1\n", rb[2]==-1);

	unit_assert("Value should be 2\n", rc[0]==2);
	unit_assert("Value should be 2\n", rc[1]==2);

	unit_assert("Value should be -2\n", rd[0]==-2);
	unit_assert("Value should be -2\n", rd[1]==-2);

	unit_assert("Value should be 1\n", re[0]==1);

	unit_assert("Value should be -1\n", rf[0]==-1);

	pass();
}

int main() {
    server_config config = { "192.168.31.78", 5200, 5201 };
    server_start(&config);

	TESTS (
		test1,
		id_table,
		message
	);

    return 0;
}
