//
//  Starcounter Lisp
//
//  Cons.cpp
//
//  The Cons from Lisp. A Cons is a pair with a left and a right value.
//  The left value is the first value of a list and the right value is the rest of the list.
//  (aka car,cdr).
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Cons.hpp"



void CONS::Print() {
    if (Pointer == 0) {
        std::cout << "()";
    }
    else {
        std::cout << " (";
        GetCons()->Car.Print();
        std::cout << " . ";
        GetCons()->Cdr.Print();
        std::cout << ")";
    }
}

Cons* CONS::AllocateCons( VALUE car, VALUE cdr ) {
        auto c = new Cons(car,cdr); // TODO! GC
        Pointer = (uint64_t)c;
    return c;
}
