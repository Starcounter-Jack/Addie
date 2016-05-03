//
//  Starcounter Lisp
//
//  Cons.cpp
//
//  The Cons from Lisp. A Cons is a pair with a left and a right value.
//  The left value is the first value of a list and the right value is the rest of the list.
//  (aka first,rest).
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Cons.hpp"







Cons* LIST::MaterializeAsCons( VALUE first, VALUE rest ) {
    Cons* c = MALLOC_HEAP(Cons); // * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));
    new (c) Cons(first,rest);  // Calling constructor
    Integer = (uint64_t)c;
    return c;
}


