#include "./unit.h"
#include <stdio.h>

const char *test1() {
	unit_assert("Here is a message", 1 == 1);

	pass();
}

int main() {

	TESTS (
		test1
	);

    return 0;
}