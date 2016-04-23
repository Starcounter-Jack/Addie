//
//  Starcounter Lisp
//
//  Vector.cpp
//
//  Persistent (immutable) vector (array).
//
//  Leaning heavily on Phil Bagwells original algorith (ideal hashes) and the implementation by
//  Chae Seong Lim. See HashTrie.h for more details.
//
//  This persistent vector/hashmap is made popular by Clojure and Rich Hickey.
//
//  Created by Joachim Wester on 22/04/16.
//
//  Copyright © 2016 Starcounter AB.
//
//



#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include "Value.hpp"

template <class T>

class Vector {
private:
//    T Items[32];
    std::vector<T> Items;
    
public:
    
    T GetAt( int t ) {
        return Items[t];
    }
    
    void SetAt( int t, VALUE v ) {
        return Items[t] = v;
    }
    
    void Append( VALUE v ) {
//        Append( v );
    }
    
};

#ifndef _PVEC_H
#define _PVEC_H

#include <stdint.h>
#define BITS 5
#define WIDTH (1 << BITS)
#define MASK (WIDTH - 1)

struct PVecNode_ {
    struct PVecNode_ *children[WIDTH];
    void *elements[WIDTH];
};

typedef struct PVecNode_ PVecNode;

typedef struct{
    PVecNode *head;
    PVecNode *tail;
    uint64_t tail_length;
    uint64_t length;
    uint64_t depth;
} PersistentVector;

PersistentVector *pvec_new(void);
void pvec_free(PersistentVector *vec);
PersistentVector *pvec_cons(PersistentVector *vec, void *data);
PersistentVector *pvec_assoc(PersistentVector *vec, uint64_t key, void *data);
void *pvec_nth(PersistentVector *vec, uint64_t key);
PersistentVector *pvec_pop(PersistentVector *vec, uint64_t key);
#endif





#endif /* Vector_hpp */
