g++ -std=c++11 -I ../../ main.cpp -o main.exe

cl /EHsc plugin1.cpp /I ..\..\ /LD /wd4503
cl /EHsc plugin2.cpp /I ..\..\ /LD /wd4503
