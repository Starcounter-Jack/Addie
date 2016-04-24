# Addie

A modern clojure based embeddable Lisp.

⚠ **DEVELOPMENT HAS JUST BEGUN. DOES NOT WORK YET!**

1. Functional first (immutable data structures)
2. Basic Clojure (script) compatibility
3. Embeddable
4. Small
5. Fast
6. Portable (ansi C/C++)
7. Simple single threaded isolates for simple embedding
8. Supports vertical parentheses to relax your brain 🍺


## Example
```clojure
⏜
if (= einstein genius)
  (print "e=mc²")
  (print "e!=mc²")
⏝
```



## Building and running

#### Linux

```bash
$ mkdir ~/addie
$ cd ~/addie
$ git clone https://github.com/Starcounter-Jack/Addie.git ./
$ make
$ ./addie
```

#### OS X

Optionally, you can use CMake to create a XCode project

```bash
$ cmake -G Xcode
```

#### Windows

We will make sure that Addie compiles using the Microsoft compiler and Visual Studio.
