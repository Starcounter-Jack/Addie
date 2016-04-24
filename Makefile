hellomake: src/Main.cpp src/Compiler.cpp
	gcc -o addie src/Main.cpp src/Compiler.cpp src/Value.cpp \
	src/Isolate.cpp src/Parser.cpp src/Compiler.cpp \
	src/Vector.cpp src/Cons.cpp -I. -std=c++11 -stdlib=libc++ -xc++ -lstdc++ -shared-libgcc
