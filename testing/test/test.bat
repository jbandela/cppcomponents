@echo off
echo Building g++ .dll
g++ -std=c++11 ../unit_test_dll.cpp -shared -o unit_test_dll.dll -fvisibility=hidden -Xlinker --enable-stdcall-fixup  -Werror -Wpedantic -pedantic-errors -Wall

echo Build MSVC .exe

cl ..\unit_test_exe.cpp ..\unit_test_exe_when_all.cpp ..\unit_test_exe_when_any.cpp ..\unit_test_exe_when_any_tuple.cpp  ..\unit_test_exe_channel.cpp .\external\googletest-read-only\src\gtest_main.cc .\external\googletest-read-only\src\gtest-all.cc /I .\external\googletest-read-only /I .\external\googletest-read-only\include /EHsc /wd4503 /nologo /W4 /WX
echo Running MSVC(exe) with g++(dll)

unit_test_exe.exe --gtest_print_time=0
echo Done running 1 of 2 tests at
time /t
del *.obj
del *.dll
del *.exe

echo Building g++ .exe
g++ -std=c++11 -U__STRICT_ANSI__ ../unit_test_exe.cpp ../unit_test_exe_when_all.cpp ..\unit_test_exe_when_any.cpp ..\unit_test_exe_when_any_tuple.cpp  ..\unit_test_exe_channel.cpp ./external/googletest-read-only/src/gtest_main.cc ./external\googletest-read-only/src/gtest-all.cc -I ./external/googletest-read-only -I ./external/googletest-read-only/include -o unit_test_exe.exe  -Werror -Wpedantic -pedantic-errors -Wall

echo Build MSVC .dll
cl /EHsc ..\unit_test_dll.cpp /LD /nologo /wd4503 /W4 /WX

echo Running g++(exe) with msvc(dll)

unit_test_exe.exe --gtest_print_time=0
echo Done running 2 of 2 tests at
time /t
del *.dll
del *.obj
del *.exe
del *.lib
del *.exp
