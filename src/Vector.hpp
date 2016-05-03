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
#include "Isolate.hpp"

template <class T>

// TODO! Replace which Bitmap Vector Trie
class PVector : public Object {
    
public:
    std::vector<T> Items;
    
    T GetAt( int t ) {
        return Items[t];
    }
    
    PVector<T>* SetAt( int t, VALUE v ) {
        if (RefCount==0) {
            // There is only one value observing this vector materialization,
            // this means that we do not have to keep the old version.
            // Let's turn the old version into the new version and forget about
            // the old version.
            this->Items[t] = v;
            return this;
        }
        // The old version needs to be persisted. Here is where we will insert the
        // bitmapped vector trie magic.
        
        PVector<T>* newVec = (PVector<T>*)MALLOC_HEAP(std::vector<T>);
        new (newVec) PVector<T>();
        newVec->Items = this->Items;
        return newVec;
    }

    template<class V>
    V Push() {
        V v;
        Items.push_back(v);
        return v;
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
