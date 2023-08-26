
# Info

Example of simple reference echo server with following advantages:
* support reading and writing operations in any order
* prevent out of memory case (limit buffer size)
* avoid unnecessary memory allocations (use circular buffer)

# Running

```shell
$ telnet 127.0.0.1 8080
Hi
Hi
```