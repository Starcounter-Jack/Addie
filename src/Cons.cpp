//
//  Cons.cpp
//  Addie
//
//  The Cons from Lisp. A Cons is a pair with a left and a right value.
//  The left value is the first value of a list and the right value is the rest of the list.
//  (aka first,rest).
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "Addie.hpp"
#include "Optimized_Array.hpp"

#ifdef USE_CONS

#include "Cons.hpp"
#include "Optimized_Array.hpp"




Cons* LIST::MaterializeAsCons( VALUE first, VALUE rest ) {
    Cons* c = MALLOC_HEAP(Cons); // * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));
    new (c) Cons(first,rest);  // Calling constructor
    Integer = (uint64_t)c;
    return c;
}
#endif

/*
#ifdef USE_ARRAY
Array* LIST::MaterializeAsArray( VALUE first, VALUE rest ) {
    auto arr = MALLOC_HEAP(Array);
    new (arr) Array( first, rest );
    Integer = (uintptr_t)arr;
    return arr;
}
#endif
*/

