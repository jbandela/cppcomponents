@echo off
echo Building g++ .dll
g++ -std=c++11 ../unit_test_dll.cpp -shared -o unit_test_dll.dll -fvisibility=hidden ../unit_test_dll.def -Xlinker --enable-stdcall-fixup

echo Build MSVC .exe

cl ..\unit_test_exe.cpp .\external\googletest-read-only\src\gtest_main.cc .\external\googletest-read-only\src\gtest-all.cc /I .\external\googletest-read-only /I .\external\googletest-read-only\include /EHsc /wd4503 /nologo

echo Running MSVC(exe) with g++(dll)

unit_test_exe.exe --gtest_print_time=0

del *.obj
del *.dll
del *.exe

echo Building g++ .exe
g++ -std=c++11 -U__STRICT_ANSI__ ../unit_test_exe.cpp ./external/googletest-read-only/src/gtest_main.cc ./external\googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe.exe

echo Build MSVC .dll
cl /EHsc ..\unit_test_dll.cpp ..\unit_test_dll.def /LD /nologo

echo Running g++(exe) with msvc(dll)

unit_test_exe.exe --gtest_print_time=0

del *.dll
del *.obj
del *.exe
del *.lib
del *.exp