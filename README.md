# Addie

A Modern Clojure based embeddable Lisp. Lightweight scripting language that is easy to port and integrate into existing C/C++ codebases. Focus on efficiency for functional programming.

*** JUST STARTED DEVELOPMENT. DOES NOT WORK YET ***


## Building and running

You need a C/C++ compiler and CMake.

### Linux

```bash
$ mkdir ~/addie
$ cd ~/addie
$ sudo apt-get install cmake
$ git clone https://github.com/Starcounter/Addie.git ./
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./addie
```

### Windows

TODO!

### OS X

Optionally, you can use CMake to create a XCode project

```bash
$ cmake -G Xcode
```
