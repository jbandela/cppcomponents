#`cppcomponents` - The C++11 Components Framework 

#### What is cppcomponents?
`cppcomponents` is a header-only, portable library that allows you to create binary C++ components that can be easily used
across different compilers and different standard library implementations.

Here is another way to think of it. You have probably heard of [C++/CX](http://en.wikipedia.org/wiki/C%2B%2B/CX) the C++ 
Component Extensions from Microsoft. This library allows you to do much of the same thing, but without language extensions,
and works on multiple compilers on Windows and Linux.



#### Why cppcomponents?

Have you ever wanted to change or upgrade compilers or standard libraries, but were hesitant because your old libraries would have to be rebuilt?

Have you wanted to use 2 compilers together(for example you want to use g++ for some code because of improved C++11 features, 
and use Visual C++ for some other code because of easier access to Windows features) but integrating them was too difficult?

Have you wanted to use different standard libraries? For example, on Linux write some code in g++ with libstdc++ and other code in clang++ with libc++?

`cppcomponents` allows you to make components, compile them, and use the resulting binaries from other compilers.

#### What compilers and operating systems are supported?

* Tested on Windows with MSVC 2013 Preview, and Mingw GCC 4.8.1 
* Tested on Ubuntu 13.04 with g++ 4.7.3 and clang 3.4 with libc++

The library would probably work on OSX with minimal effort, but I don't have a Mac.


#### What features are supported

* Constructors
* Static functions
* `std::string`, `std::vector`, `std::pair` as function parameters and return values
* Exceptions
* Properties
* Events

There is no need to link to a component made with `cppcomponents`. You just include the header file in your code, and copy the .dll or .so file to same directory as your 
executable. It does not matter what compiler or standard library was used to create the .dll or .so file, it just works.

#### What about COM and WinRT

Components written using `cppcomponents` have built in support for `IUnknown` that makes them COM compatible (and XPCOM compatible). 
However, there is no support for other specific COM interfaces and types such as IDispatch, VARIANT,IClassFactory. There are plans for a library to provide this.

With regards to WinRT, the story is the same, only more so. Like WinRT, `cppcomponents` uses the binary interface of COM.
Like WinRT, it also has factory and static interfaces in addition to object interfaces. Finally, the Events support is deliberately
compatible with WinRT.
There is a project at the repository cc_winrt that uses `cppcomponents` as a base, and builds on it with support for HSTRING and Activation Factories.
That project is still at an initial stage.


###Using under Linux

1. Clone the code from this git repository
2. Add the directory to the Include path
3. To use include `cppcomponents/cppcomponents.hpp` and optionally `cppcomponents/events.hpp` for events
4. When you compile, make sure you include `-std=c++11` and `-ldl`

####Using under Windows

1. Clone the code from this git repository
2. Add the directory to the Include path
3. To use include `cppcomponents/cppcomponents.hpp` and optionally `cppcomponents/events.hpp` for events
4. When you compile with g++, make sure you include `-std=c++11`


###Unit Tests

Unit tests are provided using the Google Test Framework.
To run the test..

1. cd to `testing/test` 
2. Make sure that g++ and clang++ are in your path on Linux, and g++ and cl are in you path on Windows
3. On Linux make sure `test.sh` has execute permissions
4. On Linux `./test.sh` and on Windows `test.bat`
5. On both, you will see 2 sets of test run. On Windows the cl and g++ are used for exe/dll and dll/exe respectively. On Linux clang++ and g++ are used.



###License

```

         Copyright John R. Bandela 2013.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
```