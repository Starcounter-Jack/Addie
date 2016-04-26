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
    auto teststr = StringReader(str,strlen(str));
    std::cout << "\n**** Parsing ****\n" << str << "\n";
    std::cout << "===>\n";
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

    std::cout << "\n**** Compiling and running ****\n";
    Compiler c;
    Interpreter i;
    STRING code = c.Compile( v );
    VALUE result = i.Interpret( code.Bytes() );

    result.Print();
    std::cout << "\n\n";
    
   // std::cout << JitX86(123);


//    CurrentIsolate->Heap.PrintStatus();
}