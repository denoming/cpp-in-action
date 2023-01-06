# Info

Using templates in module.

# Building

```shell
$ g++ -std=c++20 -fmodules-ts -c math.cpp
$ g++ -std=c++20 -fmodules-ts -c main.cpp
$ g++ -std=c++20 -fmodules-ts math.o main.o -o main
```
