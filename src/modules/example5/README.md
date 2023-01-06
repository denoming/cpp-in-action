# Info

Using module partitioning.

# Building

```shell
$ g++ -std=c++20 -fmodules-ts -c mathModule1.cpp
$ g++ -std=c++20 -fmodules-ts -c mathModule2.cpp
$ g++ -std=c++20 -fmodules-ts -c math.cpp
$ g++ -std=c++20 -fmodules-ts -c main.cpp
$ g++ -std=c++20 -fmodules-ts mathModule1.o mathModule2.o math.o main.o -o main
```
