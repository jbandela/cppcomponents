#!/bin/bash
DEFAULT_CLANG_LIB=libstdc++
clang_lib=${1:-$DEFAULT_CLANG_LIB}

echo "Building clang .so"
clang++ -std=c++11  -stdlib=$clang_lib ../unit_test_dll.cpp -shared -o unit_test_dll.so -fPIC -fvisibility=hidden -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  -Werror -Wpedantic -pedantic-errors

echo "Building g++ executable"
g++ -std=c++11 -U__STRICT_ANSI__ ../unit_test_exe.cpp ../unit_test_exe_call_by_name.cpp ../unit_test_exe_when_all.cpp ../unit_test_exe_when_any.cpp ../unit_test_exe_when_any_tuple.cpp ../unit_test_exe_channel.cpp  ./external/googletest-read-only/src/gtest_main.cc ./external/googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe -ldl -pthread  


echo "Running g++(exe) with clang++(so)"

./unit_test_exe --gtest_print_time=0

rm *.so
rm unit_test_exe


echo "Building g++ .so"

g++ -std=c++11 ../unit_test_dll.cpp -shared -o unit_test_dll.so -fPIC -fvisibility=hidden  

echo "Building clang executable"

clang++ -std=c++11 -D__STRICT_ANSI__ -stdlib=$clang_lib ../unit_test_exe.cpp ../unit_test_exe_call_by_name.cpp ../unit_test_exe_when_all.cpp ../unit_test_exe_when_any.cpp ../unit_test_exe_when_any_tuple.cpp  ../unit_test_exe_channel.cpp  ./external/googletest-read-only/src/gtest_main.cc ./external/googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe -ldl -pthread -lsupc++ -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  -Werror -Wpedantic -pedantic-errors

echo "Running clang++(exe) with g++(so)"

./unit_test_exe --gtest_print_time=0

rm *.so
rm unit_test_exe

