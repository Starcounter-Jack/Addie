//
//  Main.cpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#include "Main.hpp"
#include "Parser.hpp"
#include <iostream>


int main(int argc, const char * argv[]) {
    
    /*
     http://stackoverflow.com/questions/4956249/using-malloc-instead-of-new-and-calling-the-copy-constructor-when-the-object-is
     You'll need to use placement new after getting the raw memory from malloc.
     
     void* mem = malloc(sizeof(S));
     S* s = new (mem) S(); //this is the so called "placement new"
     When you're done with the object you have to make sure to explicitly call its destructor.
     
     s->~S();
     free(mem);
     */
    Isolate isolate;
    
    
    auto test_str = StringReader("\"Jack Wester\"");
    auto test_list = StringReader("(Jack Wester)");
    
    test_str.Read();
    Value v1 = Parser::ParseString( &isolate, &test_str );
    
    test_list.Read();
    Value v2 = Parser::ParseList( &isolate, &test_list );
    
    v1.Print();
    v2.Print();
    
    Heap.PrintStatus();

}