//
//  Cons.hpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Cons_hpp
#define Cons_hpp

#include "Cons.hpp"
#include "Value.hpp"
#include "Isolate.hpp"
#include "List.hpp"


// This list materialization is based on the classical Lisp linked list.
// The names first and rest are used instead of car and cdr as is also the
// case in Clojure.
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
        if (RefCount==0 && Rest().IsNil()) {
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
    
    
    LIST ReplaceAt( int i, VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }
    
    VALUE RemoveAt( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE InsertAt( int i, VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Concatenate( VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Reverse() final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Replace( VALUE v1, VALUE v2 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Sort( VALUE fun ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Map( VALUE fun ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE First( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Last( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    VALUE Skip( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }



};

#endif /* List_hpp */
