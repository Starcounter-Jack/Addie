//
//  Starcounter Lisp
//
//  Cons.hpp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Cons_hpp
#define Cons_hpp

#include "Cons.hpp"
#include "Value.hpp"
#include "Isolate.hpp"
#include "List.hpp"


class Cons : public List {
public:
    VALUE Car;
    VALUE Cdr;
public:
    VALUE GetCar() {
        return Car;
    }
    
    VALUE GetCdr() {
        return Cdr;
    };
    
    Cons( VALUE _car, VALUE _cdr ) {
        Car = _car;
        Cdr = _cdr;
    }
    
    
    LIST Append( VALUE elem ) {
        if (RefCount==0 && GetCdr() == NIL()) {
            // We can optimize things as we can reuse this materalization.
            // Nobody is referencing it.
//           if (GetCdr() != NIL()) {
//               throw std::runtime_error("Can only append at the end of a list");
//           }
           LIST c;
           auto x = c.MaterializeAsCons( elem, NIL() );
           Cdr = c;
           return LIST(x);
        }
        throw std::runtime_error("Not implemented yet");
    }
};

#endif /* List_hpp */
