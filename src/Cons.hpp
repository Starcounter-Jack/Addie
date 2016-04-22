//
//  Starcounter Lisp
//
//  Cons.hpp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#ifndef Cons_hpp
#define Cons_hpp

#include "Value.hpp"


class Cons {
public:
    Cons( Value first, Value rest ) {
        First = first;
        Rest = rest;
    }
    
    Value First;   // Aka CAR
    Value Rest;    // Aka CDR
};


#endif /* List_hpp */
