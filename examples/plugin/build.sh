g++ -std=c++11 -I ../../ main.cpp -o main -ldl -pthread

clang++ -std=c++11 -I ../../ plugin1.cpp -shared -fPIC -o plugin1.so
clang++ -std=c++11 -I ../../ plugin2.cpp -shared -fPIC -o plugin2.so

