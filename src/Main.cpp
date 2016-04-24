//
//  Main.cpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//
#include "Helpers.h"


#include "Main.hpp"
#include "Parser.hpp"
#include <iostream>
#include "../tests/TestHashTrie.hpp"
#include "Compiler.hpp"
#include "Interpreter.hpp"


VALUE IllustrateParse( const char* str ) {
    auto teststr = StringReader(str);
    std::cout << "\n**** Parsing ****\n" << str << "\n";
    std::cout << "===>\n\n";
    VALUE v = Parser::ParseForm( &teststr );
    v.Print();
    std::cout << "\n\n";
    return v;
}

int main(int argc, const char * argv[]) {
    
    Isolate isolate;
    CurrentIsolate = &isolate;
    
    IllustrateParse( "\"Jack Gök Wester\"" );
    IllustrateParse("(\"Jack\" \"Wester\")");
    IllustrateParse( "\n⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝" );
    VALUE v = IllustrateParse("(+ 1 1)");

    Compiler c;
    Interpreter i;
    STRING bytecode = c.Compile( v );
    VALUE result = i.Interpret( bytecode );

    std::cout << "\n**** Compiling and running ****\n";
    result.Print();
    std::cout << "\n\n";


//    CurrentIsolate->Heap.PrintStatus();
}