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

    List* Prepend( VALUE elem ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    // Override of the List interface
    List* Append( VALUE elem ) {
        if (RefCount==0 && Rest().IsNil()) {
            // We can optimize things as we can reuse this materalization.
            // Nobody is referencing it.
           LIST c;
           auto x = c.MaterializeAsCons( elem, NIL() );
           _rest = c;
           return x;
        }
        throw std::runtime_error("Not implemented yet");
    }
    
    // Override of the List interface
    VALUE GetAt( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }
    
    
    List* ReplaceAt( int i, VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }
    
    List* RemoveAt( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* InsertAt( int i, VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Concatenate( VALUE v ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Reverse() final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Replace( VALUE v1, VALUE v2 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Sort( VALUE fun ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Map( VALUE fun ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* First( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Last( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Skip( int i ) final {
        throw std::runtime_error("Not implemented yet");
    }



};

#endif /* List_hpp */
