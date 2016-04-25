# Addie

⚠ **DEVELOPMENT HAS JUST BEGUN. DOES NOT WORK YET!**
A modern clojure based embeddable Lisp.


### Update!

The interal VM bytecode is being layed out in the [wiki](https://github.com/Starcounter-Jack/Addie/wiki). Please leave a comment as work on the interpreter is about to begin.

1. Functional first (immutable data structures)
2. Basic Clojure (script) compatibility
3. Embeddable
4. Fast
5. Simple single threaded isolates for simple embedding
6. Supports vertical parentheses to relax your brain 🍺


## Example
```clojure
⏜
if (= einstein genius)
  (print "e=mc²")
  (print "e!=mc²")
⏝
```



## Building and running

Just run make to build. You can also optionally use CMake.

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
