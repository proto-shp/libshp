
add_executable(libshp_test test.c)
target_link_libraries(libshp_test libshp)

add_test(NAME libshp_test COMMAND libshp_test)
