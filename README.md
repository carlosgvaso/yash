YASH
====

**Yet Another SHell** (YASH) is a basic Linux shell built on C with minimum library dependencies.

The requirements for this software are outlined in the following document:

[EE382V - Systems Programming Project 1: yet another shell (yash)](https://docs.google.com/document/d/1XVBlxWYLyrwk0E2jrCo95J3zRjEoht-KK5qnZSFFKBE/edit?usp=sharing)


Building
--------

For a standard build of the project run the following command from the root folder of the project:

```console
$ make
```

Other building options are as follows:

1. `$ make yash`: build the project normally.

2. `$ make debug`: build the roject with debugging symbols for GDB.

3. `$ make clean`: remove object files from previous builds.


Running
-------

After building the project the `yash` executable should appear inside the `/bin/` directory. To run the shell use the following command from the same directory where the executable is placed:

```console
$ ./yash
```

More Information
----------------

The project is written in C using the C11 standard. It has been compiled and tested using the following environment:

* Processor: Intel Core i5-3337U CPU @ 1.80GHz × 4
* Memory: 6GB
* OS: Ubuntu 19.04 Desktop x86_64 GNU/Linux
* Linux Kernel: 5.0.038-generic
* Compiler: GCC 8.3.0
* Debugger: GDB 8.2.91.20190405-git
