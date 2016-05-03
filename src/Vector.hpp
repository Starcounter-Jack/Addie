//
//  Starcounter Lisp
//
//  Vector.cpp
//
//  Persistent (immutable) vector (array).
//
//  Leaning heavily on Phil Bagwells original algorith (ideal hashes).
//
//  I will look for a good implementation.
//
//  This use of persistent vector/hashmap is made popular by Clojure and Rich Hickey.
//
//  Created by Joachim Wester on 22/04/16.
//
//  Copyright © 2016 Starcounter AB.
//



#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include "List.hpp"


// TODO! Replace which Bitmap Vector Trie
class PVector : public List {
    
public:
    

    
    /*
     template <class T>
    
     std::vector<T> Items;
     T GetAt( int t ) {
     return Items[t];
     }
     
    PVector<T>* ReplaceAt( int t, VALUE v ) {
        if (RefCount==1) {
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
     */

};



#endif /* Vector_hpp */
