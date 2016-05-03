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
#include "Isolate.hpp"
#include "List.hpp"


class Cons : public List {
public:
    VALUE First;
    VALUE Rest;
public:
    VALUE GetFirst() {
        return First;
    }
    
    VALUE GetRest() {
        return Rest;
    };
    
    
    Cons( VALUE _first, VALUE _rest ) {
        First = _first;
        Rest = _rest;
    }
    
    int Count() {
        throw std::runtime_error("Not implemented yet");
    }

    LIST Prepend( VALUE elem ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    LIST Append( VALUE elem ) {
        if (RefCount==0 && GetRest() == NIL()) {
            // We can optimize things as we can reuse this materalization.
            // Nobody is referencing it.
//           if (GetRest() != NIL()) {
//               throw std::runtime_error("Can only append at the end of a list");
//           }
           LIST c;
           auto x = c.MaterializeAsCons( elem, NIL() );
           Rest = c;
           return LIST(x);
        }
        throw std::runtime_error("Not implemented yet");
    }
    
    VALUE GetAt( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }
    
    
    LIST SetAt( int i, VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }

};

#endif /* List_hpp */
