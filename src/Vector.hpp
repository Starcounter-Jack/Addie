//
//  Starcounter Lisp
//
//  Vector.cpp
//  Implementation of a Bitmapped Vector Trie
//  Clojure Inspired Immutable/Persistent Vector.
//  Needs unit tests badly.
//
//  https://www.youtube.com/watch?v=pNhBQJN44YQ
//  http://www.infoq.com/presentations/Value-Identity-State-Rich-Hickey
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include "Value.hpp"

template <class T>

// TODO! Not really reflecting a bitmapped vector trie just yet ;-) !!!
class Vector {
private:
//    T Items[32];
    std::vector<T> Items;
    
public:
    
    T GetAt( int t ) {
        return Items[t];
    }
    
    void SetAt( int t, Value v ) {
        return Items[t] = v;
    }
    
    void Append( Value v ) {
//        Append( v );
    }
    
};


#endif /* Vector_hpp */
