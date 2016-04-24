# Addie

Clojure based embeddable Lisp.

1. Functional
2. Some basic Clojure compatibility
3. Small
4. Embeddable
5. Minimal startup time
6. Portable
7. Fast enough

### Example
```clojure
⏜
if (= einstein genius)
  (print "e=mc²")
  (print "e!=mc²")
⏝
```

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
