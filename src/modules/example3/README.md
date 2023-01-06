# Info

Using private module fragment.

# Building

```shell
$ g++ -std=c++20 -fmodules-ts -c math.cpp
$ g++ -std=c++20 -fmodules-ts -c main.cpp
$ g++ -std=c++20 -fmodules-ts main.o math.o -o main
```
