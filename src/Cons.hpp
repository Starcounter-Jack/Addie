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
private:
    VALUE _first;
    VALUE _rest;
public:
    VALUE First() {
        return _first;
    }
    
    VALUE Rest() {
        return _rest;
    };
    
    
    Cons( VALUE first, VALUE rest ) {
        _first = first;
        _rest = rest;
    }
    
    int Count() {
        throw std::runtime_error("Not implemented yet");
    }

    LIST Prepend( VALUE elem ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    LIST Append( VALUE elem ) {
        if (RefCount==0 && Rest() == NIL()) {
            // We can optimize things as we can reuse this materalization.
            // Nobody is referencing it.
//           if (GetRest() != NIL()) {
//               throw std::runtime_error("Can only append at the end of a list");
//           }
           LIST c;
           auto x = c.MaterializeAsCons( elem, NIL() );
           _rest = c;
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
