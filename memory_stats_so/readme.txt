1. The shared library is implemented in malloc.h and malloc.cc. my_malloc.h
defines the public interfaces of the shared library libmy_malloc.so.

2. A read-write lock(write-preference) is also implemented in my shared
library to support multi-threading.

3. The test program is implemented in test.cc, with 3 threads for memory
allocation and free, respectively.

4. To try out my code, please
    make
    ./my_malloc_test

Then in a new terminal run command
    kill -SIGQUIT `pidof my_malloc_test`
.

The sample output is like:

>>>>>>>>>>>>> Tue Aug 14 21:17:04 2018 <<<<<<<<<<<
Overall stats:
103831 overall allocations(246 MB) since start
93 MB current total allocated size

Current allocations by size:
0 - 4 bytes: 34
4 - 8 bytes: 23
8 - 16 bytes: 53
16 - 32 bytes: 118
32 - 64 bytes: 267
64 - 128 bytes: 499
128 - 256 bytes: 1035
256 - 512 bytes: 1999
512 - 1024 bytes: 4015
1024 - 2048 bytes: 7963
2048 - 4096 bytes: 16134
4096 + bytes: 6992

Current allocations by age:
< 1 sec: 0
< 10 sec: 0
< 100 sec: 0
< 1000 sec: 39132
> 10000 sec: 0

