#!/bin/bash

echo "Building clang .so"
clang++ -std=c++11 -stdlib=libc++ ../unit_test_dll.cpp -shared -o unit_test_dll.so -fPIC -fvisibility=hidden

echo "Building g++ executable"
g++ -std=c++11 -U__STRICT_ANSI__ ../unit_test_exe.cpp ./external/googletest-read-only/src/gtest_main.cc ./external/googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe -ldl -pthread 


echo "Running g++(exe) with clang++(so)"

./unit_test_exe --gtest_print_time=0

rm *.so
rm unit_test_exe


echo "Building g++ .so"

g++ -std=c++11 ../unit_test_dll.cpp -shared -o unit_test_dll.so -fPIC -fvisibility=hidden

echo "Building clang executable"

clang++ -std=c++11 -stdlib=libc++ -U__STRICT_ANSI__ ../unit_test_exe.cpp ./external/googletest-read-only/src/gtest_main.cc ./external/googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe -ldl -pthread -lsupc++ 
echo "Running clang++(exe) with g++(so)"

./unit_test_exe --gtest_print_time=0

rm *.so
rm unit_test_exe

