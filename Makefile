hellomake: src/Main.cpp src/Compiler.cpp
	gcc -o addie src/Main.cpp src/Compiler.cpp src/Value.cpp \
	src/Isolate.cpp src/Reader.cpp src/Interpreter.cpp \
	-O -I. -std=c++11 -stdlib=libc++ -xc++ -lstdc++ -shared-libgcc
