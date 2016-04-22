//
//  Isolate.hpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#ifndef Isolate_hpp
#define Isolate_hpp

#include "Value.hpp"

class Isolate {
    
public:
    Value EmptyList;
    Value NIL;
    
    Isolate()
    {
        NIL.Primitive.Integer = 0;
    }
};

#endif /* Isolate_hpp */
