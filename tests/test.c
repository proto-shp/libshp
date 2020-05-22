#include "./unit.h"
#include <stdio.h>

#include "libshp/csocket.h"

const char *test1() {
    csocket sock;
    csocket_udp_init(&sock, "", 5400);
	pass();
}

int main() {

	TESTS (
		test1
	);

    return 0;
}