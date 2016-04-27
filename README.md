# Addie



<table border=0>
<tr>
<td>
<img src="https://github.com/Starcounter-Jack/Addie/blob/master/docs/addie2.gif">
</td>
<td valign="top"><h4>A modern clojure style embeddable Lisp.</h4>
1. Pure functional<br>
2. Based on Clojure<br>
3. Embeddable<br>
4. Fast<br>
5. Simple single threaded isolates for simple embedding<br>
6. Supports vertical parentheses to relax your brain 🍺<br>
<br>
⚠ DEVELOPMENT HAS JUST BEGUN.<br>⚠ DOES NOT WORK YET!<br>
</td>
</tr>
<table>

## Update!

The interal VM bytecode is being layed out in the [wiki](https://github.com/Starcounter-Jack/Addie/wiki). Please leave a comment as work on the interpreter is about to begin.




### Example
```clojure
⏜
if (= einstein genius)
  (print "e=mc²")
  (print "e!=mc²")
⏝
```



### Building and running

Just run make to build. You can also optionally use CMake.

##### Linux

```bash
$ mkdir ~/addie
$ cd ~/addie
$ git clone https://github.com/Starcounter-Jack/Addie.git ./
$ make
$ ./addie
```

##### OS X

Optionally, you can use CMake to create a XCode project

```bash
$ cmake -G Xcode
```

##### Windows

We will make sure that Addie compiles using the Microsoft compiler and Visual Studio.
