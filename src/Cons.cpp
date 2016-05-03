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
#include <sstream>



std::string LIST::Print() {
    std::ostringstream res;
    Cons* self = GetCons();
    char startParen;
    char endParen;
    if (IsClassicalParenthesis()) {
        startParen = '(';
        endParen = ')';
    } else {
        startParen = '[';
        endParen = ']';
    }
    if (Integer == 0) {
        res << startParen << endParen;
        return res.str();
    }
    if (self->GetCdr().IsCons()) {
        res << startParen;
        res << self->GetCar().Print();
        VALUE next = self->GetCdr();
        while (next.IsCons()) {
            res << " ";
            Cons* pnext = (Cons*)next.OtherBytes();
            res << pnext->GetCar().Print();
            next = pnext->GetCdr();
        }
        if (!next.IsNil()) {
            res << " . ";
            res << next.Print();
        }
        res << endParen;
    }
    else {
        res << startParen;
        res << self->GetCar().Print();
        if (!self->GetCdr().IsNil()) {
           res << " . ";
           res << self->GetCdr().Print();
        }
        res << endParen;
    }
    return res.str();
}

LIST LIST::Append( VALUE elem ) {
    return GetCons()->Append( elem );
}


Cons* LIST::__allocateCons( VALUE car, VALUE cdr ) {
    Cons* c = MALLOC_HEAP(Cons); // * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));
    new (c) Cons(car,cdr);  // Calling constructor
    Integer = (uint64_t)c;
    return c;
}
