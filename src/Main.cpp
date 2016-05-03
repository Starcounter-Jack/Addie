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
#include "Compiler.hpp"
#include "Interpreter.hpp"
#include "Array.hpp"

Array<VALUE>* CreateSimpleArray() {
    Array<VALUE>* arr = Array<VALUE>::__beginWrite();
    arr->__write(INTEGER(123));
    arr->__write(INTEGER(789));
    arr->__endWrite();
    assert( arr->Count() == 2 );
    assert( arr->GetAt(0).Integer == 123 );
    assert( arr->GetAt(1).Integer == 789 );
    return arr;
}

void TestArrays() {
    Array<VALUE>* arr = CreateSimpleArray();
    arr->RefCount++;
    List* arr2 = arr->ReplaceAt(1,INTEGER(456));
    assert( arr->GetAt(1).Integer == 789 );
    assert( arr2->GetAt(1).Integer == 456 );
}


VALUE IllustrateParse( const char* str ) {
    std::cout << "\n**** Parsing ****\n" << str << "\n";
    std::cout << "===>\n";
    
    auto teststr = StringReader(str,strlen(str));
    VALUE v = Parser::ParseForm( &teststr );
    std::cout << v.Print();
    std::cout << "\n";
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
    VALUE v;
    
    assert( sizeof(VALUE) == sizeof(uintptr_t) ); // If we are on a 64 bit machine we want 64
                                                  // bit values
    assert( sizeof(Instruction) == 4 );           // Our byte instructions are 32 bit
    
    TestParse( "﹇﹈","[]");
    TestParse( "⎴ ⎵ ","[]");
    TestParse( "⏜⏝","()");
//    TestParse( "⏞ ⏟","{}");
//    TestParse("︷ ︸","{}");
    TestParse( "\"Jack Gök Wester\"", "\"Jack Gök Wester\"" );
    TestParse("(\"Jack\" \"Wester\")","(\"Jack\" \"Wester\")");
    TestParse( "⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝",
                    "(if (= einstein genius) (print \"e=mc²\") (print \"e!=mc²\"))");
    v = TestParse("⏜\n   ⏜\n   defn pow [n] \n      ⏜\n      fn [x]\n         (apply * (repeat n x))\n      ⏝\n   ⏝\n   (def ² (pow 2))\n   (def ³ (pow 3))\n⏝",
          "((defn pow [n[n] (fn [x[x] (apply * (repeat n x)))) (def ² (pow 2)) (def ³ (pow 3)))");
    v = TestParse("⏜\nlet ﹇\n    a 10\n    b 20\n    ﹈\n    (print (+ a b))\n⏝",
                  "(let [a 10 b 20] (print (+ a b)))");
    
    TestArrays();
   
    
    Compilation* code = Compiler::Compile( v );
    STRINGOLD str = Compiler::Disassemble(code);
    
    std::cout << str.ToString();

    Continuation c = Interpreter::Interpret(code);
    
    Interpreter::Interpret(c);
    std::cout << "\n\n";

}