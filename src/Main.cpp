//
//  Main.cpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//


#include "VM.hpp"
#include "Reader.hpp"
#include "Compiler.hpp"
#include <iostream>

using namespace Addie::Internals;


#ifdef USE_INTARRAY

#include "Optimized_IntArray.hpp"



IntArray<int>* CreateSimpleArray() {
    IntArray<int>* arr = IntArray<int>::__beginWrite();
    arr->__write(123);
    arr->__write(789);
    arr->__endWrite();
    assert( arr->Count() == 2 );
    assert( arr->GetAt(0).Integer == 123 );
    assert( arr->GetAt(1).Integer == 789 );
    return arr;
}


void TestIntArrays() {
    IntArray<int>* arr = CreateSimpleArray();
    arr->RefCount += 2;
    List* arr2 = arr->ReplaceAt(1,INTEGER(456));
    assert( arr->GetAt(1).Integer == 789 );
    assert( arr2->Count() == 2 );
    assert( arr2->GetAt(0).Integer == 123 );
    assert( arr2->GetAt(1).Integer == 456 );
}
#endif


void TestAppend() {
    
    std::cout << "Test Append:";
    LIST str(QString,INTEGER(74));
    str.CheckIntegrety();
    LIST str2 = str.Append(INTEGER(97));
    LIST str3 = str2.Append(INTEGER(99));
    LIST str4 = str3.Append(INTEGER(107));
    std::cout << str4.Print();
    std::cout << "\n";
    assert( strcmp("\"Jack\"",str4.Print().c_str()) == 0 );
    
}



void TestPrepend() {
    
    std::cout << "Test Prepend:";
    LIST str(QString,INTEGER(107));
    str.CheckIntegrety();
    LIST str2 = str.Prepend(INTEGER(99));
    LIST str3 = str2.Prepend(INTEGER(97));
    LIST str4 = str3.Prepend(INTEGER(74));
    std::cout << str4.Print();
    std::cout << "\n";
    assert( strcmp("\"Jack\"",str4.Print().c_str()) == 0 );
    
}

void TestArrays() {
#ifdef USE_INTARRAY
    TestIntArrays();
#endif
    TestAppend();
    TestPrepend();
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
    const char* output = v.Print().c_str();
    assert( strcmp( expectedOutput, output ) == 0 );
    return v;
}

VALUE TestParse( const char* input, int expectedCount, const char* expectedOutput ) {
    VALUE x = IllustrateParse( input );
    LIST v;
    v.Whole = x.Whole;
    int actualCount = v.GetList()->Count();
    const char* output = v.Print().c_str();
    assert( actualCount == expectedCount );
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

    TestArrays();
    
    TestParse( "﹇﹈","[]");
    TestParse( "⎴ ⎵ ","[]");
    TestParse( "⏜⏝","()");
//    TestParse( "⏞ ⏟","{}");
//    TestParse("︷ ︸","{}");
    TestParse( "\"Jack\"", 4, "\"Jack\"" );
    TestParse("(\"Jack\" \"Wester\")", 2, "(\"Jack\" \"Wester\")");
    TestParse( "⏜\nif (= einstein genius)\n  (print \"e=mc²\")\n  (print \"e!=mc²\")\n⏝",
                    "(if (= einstein genius) (print \"e=mc²\") (print \"e!=mc²\"))");
    v = TestParse("⏜\n   ⏜\n   defn pow [n] \n      ⏜\n      fn [x]\n         (apply * (repeat n x))\n      ⏝\n   ⏝\n   (def ² (pow 2))\n   (def ³ (pow 3))\n⏝",
          "((defn pow [n] (fn [x] (apply * (repeat n x)))) (def ² (pow 2)) (def ³ (pow 3)))");

    v = TestParse("⏜\nlet ﹇\n    a 10\n    b 20\n    ﹈\n    (print (+ a b))\n⏝",
                  "(let [a 10 b 20] (print (+ a b)))");
    

    //v = TestParse( "[1 2 3 4]", "[1 2 3 4]" );
    
    Compilation* code = Compiler::Compile( v );
    STRINGOLD str = Compiler::Disassemble(code);
    
    std::cout << str.ToString();

    Continuation c = Interpreter::Interpret(code);
    
    Interpreter::Interpret(c);
    std::cout << "\n\n";

}