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



std::string CONS::Print() {
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
    if (self->Cdr.IsCons()) {
        res << startParen;
        res << self->Car.Print();
        VALUE next = self->Cdr;
        while (next.IsCons()) {
            res << " ";
            Cons* pnext = (Cons*)next.OtherBytes();
            res << pnext->Car.Print();
            next = pnext->Cdr;
        }
        if (!next.IsNil()) {
            res << " . ";
            res << next.Print();
        }
        res << endParen;
    }
    else {
        res << startParen;
        res << self->Car.Print();
        if (!self->Cdr.IsNil()) {
           res << " . ";
           res << self->Cdr.Print();
        }
        res << endParen;
    }
    return res.str();
}

// Snoc is the reverse of Cons. Stolen from Emacs terminology. Bang! is because
// this should only be done on mutable lists.
CONS CONS::SnocBANG( VALUE elem ) {
    Cons* cons = GetCons();
    if (cons->Cdr != NIL()) {
        throw std::runtime_error("Can only append at the end of a list");
    }
    CONS c;
    auto x = c.__allocateCons( elem, NIL() );
    GetCons()->Cdr = c;
    return CONS(x);
}


Cons* CONS::__allocateCons( VALUE car, VALUE cdr ) {
    Cons* c = MALLOC_HEAP(Cons); // * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));
    new (c) Cons(car,cdr);  // Calling constructor
    Integer = (uint64_t)c;
    return c;
}
