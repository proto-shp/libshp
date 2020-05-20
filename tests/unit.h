#include <stdio.h>

#define pass() do { printf("\033[0;33mPASS: %s\n", __func__); return 0; } while (0);
#define fail() printf("\033[0;31mFAIL: %s\n", __func__)

#define fail_msg(msg) printf("\033[1;31m%s\n", msg);
#define pass_all() printf("\033[1;32mAll Tests Passes\n")

#define as_str(s) #s
#define concat(s1, s2) "Message: "#s1"  Test: "#s2
#define unit_assert(message, test) do { if (!(test)){ fail(); \
							return concat(message, test);} } while (0)
#define unit_run(test) do { const char *message = test(); tests_run++; \
                                        if (message) return message; } while (0)

#define TEST_HEADER printf("\033[1;34mRunning Tests For: %s\n", __FILE__);
#define TESTS(...) do { TEST_HEADER unit __tests[] = {__VA_ARGS__}; \
			const char *__msg = run(__tests, sizeof(__tests) / sizeof(unit)); \
			if (__msg != 0) { fail_msg(__msg); } else { pass_all(); } } while(0)

typedef const char*(*unit)();
int tests_run = 0;

// Run all of the tests provided, return message on error else 0
const char *run(unit *fns, int num_tests) {
    for (int i = 0; i < num_tests; i++) {
        unit_run(fns[i]);
    }

    return 0;
}