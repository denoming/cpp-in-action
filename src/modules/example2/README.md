# Info

Splitting a module into several module implementation units.

# Building

```shell
$ g++ -std=c++20 -fmodules-ts -c math.cpp
$ g++ -std=c++20 -fmodules-ts -c main.cpp
$ g++ -std=c++20 -fmodules-ts -c mathAdd.cpp
$ g++ -std=c++20 -fmodules-ts -c mathMul.cpp
$ g++ -std=c++20 -fmodules-ts main.o math.o mathAdd.o mathMul.o -o main
```
