//
//  Main.cpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//
#include "Helpers.h"


#include "Main.hpp"
#include "Reader.hpp"
#include <iostream>
#include "../tests/TestHashTrie.hpp"
#include "Compiler.hpp"
#include "Interpreter.hpp"


VALUE IllustrateParse( const char* str ) {
    std::cout << "\n**** Parsing ****\n" << str << "\n";
    std::cout << "===>\n";
    
    auto teststr = StringReader(str,strlen(str));
    VALUE v = Parser::ParseForm( &teststr );
    std::cout << v.Print();
    std::cout << "\n\n";
    return v;
}

int main(int argc, const char * argv[]) {
    
    Isolate isolate;
    CurrentIsolate = &isolate;
    
//    std::cout << "\nSize of value:" << sizeof(VALUE);
//    std::cout << "\nSize of instruction:" << sizeof(Instruction) << "\n\n";
    
    IllustrateParse( "\"Jack Gök Wester\"" );
    IllustrateParse("(\"Jack\" \"Wester\")");
    IllustrateParse( "⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝" );
    VALUE v = IllustrateParse("⏜\n   ⏜\n   defn pow [n] \n      ⏜\n      fn [x]\n         (apply * (repeat n x))\n      ⏝\n   ⏝\n   (def ² (pow 2))\n   (def ³ (pow 3))\n⏝");

    v = IllustrateParse("⏜\nlet ⎴\n    a 10\n    b 20\n    ⎵\n    (print (+ a b))\n⏝");
    

    
    Compilation* code = Compiler::Compile( v );
    STRING str = Compiler::Disassemble(code);
    
    
    std::cout << str.ToString();

    Continuation c = Interpreter::Interpret(code);
    
    // str = Compiler::Disassemble(code);
    
    
    //std::cout << str.ToString();
    
    Interpreter::Interpret(c);
    std::cout << "\n\n";

}