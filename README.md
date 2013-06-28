#Cross Compiler Call
##Cross Compiler Interfaces and Cross Compiler Components

#### Why Cross Compiler Interfaces and Components?

Have you ever wanted to change or upgrade compilers or standard libraries, but were hesitant because your old libraries would have to be rebuilt?

Have you wanted to use 2 compilers together(for example you want to use g++ for some code because of improved C++11 features, and use Visual C++ for some other code because of easier access to Windows features) but integrating them was too difficult?

Cross Compiler Interfaces and Components allow you to make components that allow you to write C++ code in 1 compiler and call it from another

* Calls can be made across dll boundaries even if the exe and dll are created with different compilers
* Tested on Windows with MSVC 2012 Milan .exe and mingw gcc 4.7.2 .dll
* Ubuntu 12.10 with gcc 4.7.2 for executable and clang 3.2 (from official llvm binaries) for .so
* Passing and returning std::vector, std::string, and std::pair are supported
* Exceptions are also supported


###Runing under Linux
####Tested under Ubuntu with  GCC 4.7.2 and clang 3.2 (official llvm binaries)

Use the following to build a.out using g++

```
g++ -std=c++11 demo.cpp -ldl
```

Use the following to build DemoDLL.so

```
clang++ -std=c++11 DemoDLL.cpp -shared -o DemoDLL.so -fPIC -ldl
```
Run

```
./a.out
```

###Running under Windows
####Tested under Windows with MSVC 2012 Milan and mingw gcc 4.7.2 (using the version by STL at nuwen.net)

Under windows build the executable with
MSVC 2012 Milan - the version should be 17.00.51025

```
cl demo.cpp /EHsc 
```

Then build the dll with mingw gcc (tested with gcc 4.7.2 from nuwen.net)

```
g++ -std=c++11 DemoDLL.cpp -shared -o DemoDLL.dll DemoDLL.def
```

Run

```
demo.exe
```
###Unit Tests
Unit tests using boost unit test framework are provided
To run under Windows use the provided sln file

#### Running Under Linux
To run the unit tests on Linux, you will need boost unit_test_framework

Prior to this, boost was installed using sudo apt-get install
To build the test exe run the following command

```
g++ unit_test_exe.cpp -std=c++11 -ldl -lboost_unit_test_framework -DBOOST_TEST_DYN_LINK -o unit_test
```

To build the dynamic library for the unit_test

```
clang++ -std=c++11 unit_test_dll.cpp -o unit_test_dll.so -shared -fPIC -ldl
```

Run

```
./unit_test
```

###License

```

         Copyright John R. Bandela 2012.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
```