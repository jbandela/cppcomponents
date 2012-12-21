This repository is in preparation for a series of planned blog posts at
http://jrb-programming.blogspot.com/

In brief this allows for a relatively easy to define and use interfaces
that allows calls to be made across dll boundaries even if the exe and dll are created with different compilers
Tested on windows with MSVC 2012 Milan .exe and mingw gcc 4.7.2 .dll
And on Ubuntu 12.10 with gcc 4.7.2 for executable and clang 3.1 (from ppa:kxstudio-team/builds) for .so

passing and return std::vector and std::string are supported
Exceptions are also supported


Tested under Ubuntu with  GCC 4.7.2 and clang 3.1 (obtained from ppa:kxstudio-team/builds)
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
cl demo.cpp /EHsc Ole32.lib

Then build the dll with mingw gcc (tested with gcc 4.7.2 from nuwen.net)
g++ -std=c++11 DemoDLL.cpp -shared -o DemoDLL.dll -lole32 DemoDLL.def

Run
demo.exe
