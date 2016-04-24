# Addie

A modern clojure based embeddable Lisp.

1. Functional first (immutable data structures)
2. Basic Clojure compatibility
3. V8 semantics (multiple single threaded isolates)
3. Small
4. C/C++ Embeddable
5. Minimal startup time
6. Portable (ansi C/++)
7. Fast enough

*** DEVELOPMENT HAS JUST BEGUN. DOES NOT WORK YET! ***

### Example
```clojure
⏜
if (= einstein genius)
  (print "e=mc²")
  (print "e!=mc²")
⏝
```



## Building and running

You need a C/C++ compiler and CMake.

### Linux

```bash
$ mkdir ~/addie
$ cd ~/addie
$ sudo apt-get install cmake
$ git clone https://github.com/Starcounter-Jack/Addie.git ./
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
