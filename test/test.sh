#!/bin/bash

echo "Building clang .so"
clang++ -std=c++11 -stdlib=libc++ ../unit_test_dll.cpp -shared -o unit_test_dll.so -fvisibility=hidden

echo "Building g++ executable"
g++ -std=c++11 -U__STRICT_ANSI__ ../unit_test_exe.cpp ./external/googletest-read-only/src/gtest_main.cc ./external\googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe


echo "Running g++(exe) with clang++(so)"

./unit_test_exe.exe --gtest_print_time=0

rm *.o
rm *.so
rm unit_test_exe

