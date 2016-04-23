//
//  Isolate.hpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Isolate_hpp
#define Isolate_hpp

#include "Value.hpp"
#include <map>
#include <vector>

class Isolate {
    
public:
    std::map<std::string,uint16_t> SymbolsIds;  // { firstname:1, lastname:2, foo:3, bar:4 }
    std::vector<std::string> SymbolStrings;     // [ "firstname", "lastname", "foo", "bar" ]
    
    VALUE EmptyList;
    NIL Nil;
    
    Isolate()
    {
    }
};

#endif /* Isolate_hpp */
