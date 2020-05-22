# libshp

## Build Instructions
cmake -H. -Bbuild
cmake --build buil

tests will be build into the /build/tests directory so you can run them as:
./build/tests/{test_name}

There is a CMakeLists.txt file in the tests directory. You can add tests to this file by adding 
another executable.
