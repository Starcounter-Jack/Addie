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


void IllustrateParse( const char* str ) {
    auto teststr = StringReader(str);
    std::cout << "\n\n**** Parsing ****\n" << str << "\n";
    std::cout << "===>\n\n";
    VALUE v3 = Parser::ParseForm( &teststr );
    v3.Print();
    std::cout << "\n";
}

int main(int argc, const char * argv[]) {
    
    Isolate isolate;
    CurrentIsolate = &isolate;
    
    IllustrateParse( "\"Jack Gök Wester\"" );
    IllustrateParse("(\"Jack\" \"Wester\")");
    IllustrateParse( "\n⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝" );
    
//    CurrentIsolate->Heap.PrintStatus();
}