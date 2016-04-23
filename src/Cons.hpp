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


class Cons;

class CONS : public VALUE {
public:
    CONS() {
        HType = TCons;
        IsHeapObject = true;
        Pointer = 0;
    };
    
    bool IsEmptyList() {
        return Pointer == 0;
    }
    
    void Print();
    
    Cons* GetCons() {
        return (class Cons*)Pointer;
    }
    
    Cons* AllocateCons( VALUE car, VALUE cdr );
    
    
};


class Cons {
public:
    Cons( VALUE _car, VALUE _cdr ) {
        Car = _car;
        Cdr = _cdr;
    }
    
    VALUE Car;   // Aka CAR
    VALUE Cdr;    // Aka CDR
};



#endif /* List_hpp */
