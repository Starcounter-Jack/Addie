# Addie

⚠ DEVELOPMENT HAS JUST BEGUN. ⚠ DOES NOT WORK YET!<br>

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
<img src="https://github.com/Starcounter-Jack/Addie/blob/master/docs/example2.png" width="382px"><br>
</td>
</tr>
<table>




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

You can use the same instructions as for the Linux build. Optionally, you can use CMake to create a XCode project

```bash
$ cmake -G Xcode
```

##### Windows

We will make sure that Addie compiles using the Microsoft compiler and Visual Studio.
