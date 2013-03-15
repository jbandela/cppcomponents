This repository is in preparation for a series of planned blog posts at
http://jrb-programming.blogspot.com/


In brief this allows for easy to define,implement and use interfaces
that allows calls to be made across dll boundaries even if the exe and dll are created with different compilers
Tested on windows with MSVC 2012 Milan .exe and mingw gcc 4.7.2 .dll
And on Ubuntu 12.10 with gcc 4.7.2 for executable and clang 3.2 (from official llvm binaries) for .so

passing and return std::vector, std::string, and std::pair are supported
Exceptions are also supported


Tested under Ubuntu with  GCC 4.7.2 and clang 3.2 (official llvm binaries)
Use the following to build a.out using g++

g++ -std=c++11 demo.cpp -ldl

Use the following to build DemoDLL.so

clang++ -std=c++11 DemoDLL.cpp -shared -o DemoDLL.so -fPIC -ldl

Run
./a.out

Tested under Windows with MSVC 2012 Milan
and mingw gcc 4.7.2 (using the version by STL at nuwen.net)

Under windows build the executable with
MSVC 2012 Milan - the version should be 17.00.51025
cl demo.cpp /EHsc 

Then build the dll with mingw gcc (tested with gcc 4.7.2 from nuwen.net)
g++ -std=c++11 DemoDLL.cpp -shared -o DemoDLL.dll DemoDLL.def

Run
demo.exe


To run the unit tests on Linux, you will need boost unit_test_framework

Prior to this, boost was installed using sudo apt-get install
To build the test exe run the following command

g++ unit_test_exe.cpp -std=c++11 -ldl -lboost_unit_test_framework -DBOOST_TEST_DYN_LINK -o unit_test

To build the dynamic library for the unit_test

clang++ -std=c++11 unit_test_dll.cpp -o unit_test_dll.so -shared -fPIC -ldl

run ./unit_test

License

         Copyright John R. Bandela 2012.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)