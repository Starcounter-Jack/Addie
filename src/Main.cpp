//
//  Main.cpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//
#include "Helpers.h"


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

VALUE TestParse( const char* input, const char* expectedOutput ) {
    VALUE v = IllustrateParse( input );
    const char* output = v.Print().c_str();;
    assert( strcmp( expectedOutput, output ) == 0 );
    return v;
}


int main(int argc, const char * argv[]) {
    
    Isolate isolate;
    CurrentIsolate = &isolate;
    
    assert( sizeof(VALUE) == sizeof(uintptr_t) ); // If we are on a 64 bit machine we want 64
                                                  // bit values
    assert( sizeof(Instruction) == 4 );           // Our byte instructions are 32 bit
    
//    IllustrateParse( "[]" );
    TestParse( "﹇﹈","[]");
    TestParse( "⎴ ⎵ ","[]");
    TestParse( "⏜⏝","()");
//    TestParse( "⏞ ⏟","{}");
//    TestParse("︷ ︸","{}");
    IllustrateParse( "\"Jack Gök Wester\"" );
    IllustrateParse("(\"Jack\" \"Wester\")");
    IllustrateParse( "⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝" );
   
    VALUE v = TestParse("⏜\n   ⏜\n   defn pow [n] \n      ⏜\n      fn [x]\n         (apply * (repeat n x))\n      ⏝\n   ⏝\n   (def ² (pow 2))\n   (def ³ (pow 3))\n⏝",
          "((defn pow [n[n] (fn [x[x] (apply * (repeat n x)))) (def ² (pow 2)) (def ³ (pow 3)))"

                        );

    v = IllustrateParse("⏜\nlet ﹇\n    a 10\n    b 20\n    ﹈\n    (print (+ a b))\n⏝");
   
    
    
    
    
    Compilation* code = Compiler::Compile( v );
    STRINGOLD str = Compiler::Disassemble(code);
    
    
    std::cout << str.ToString();

    Continuation c = Interpreter::Interpret(code);
    
    // str = Compiler::Disassemble(code);
    
    
    //std::cout << str.ToString();
    
    Interpreter::Interpret(c);
    std::cout << "\n\n";

}