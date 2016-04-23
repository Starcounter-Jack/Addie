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


int main(int argc, const char * argv[]) {
    
    Isolate isolate;
    
    std::string str1 = "\"Jack Wester\"";
    std::string str2 = "(\"Jack\" \"Wester\")";
    
    auto test_str = StringReader(str1);
    auto test_list = StringReader(str2);
    
    //test_str.Read();
    //Value v1 = Parser::ParseString( &isolate, &test_str );
    std::cout << "\nParsing: " << str1 << "\n";
    VALUE v1 = Parser::ParseForm( &isolate, &test_str );
    v1.Print();
    std::cout << "\n";
    
    std::cout << "\nParsing: " << str2 << "\n";
    VALUE v2 = Parser::ParseForm( &isolate, &test_list );
    v2.Print();
    std::cout << "\n";
    
    Heap.PrintStatus();
    
//    TestHashTrie();

}