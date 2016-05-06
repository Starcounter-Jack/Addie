//
//  Cons.hpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifdef USE_CONS

#ifndef Cons_hpp
#define Cons_hpp

#include "VM.hpp"
#include "List.hpp"
#include "Cons.hpp"

namespace Addie {
    namespace Internals {
    
// This list materialization is based on the classical Lisp linked list.
// The names first and rest are used instead of car and cdr as is also the
// case in Clojure. A major difference is that Addie sees both First and Rest
// as functions that are lazily evaluated. If the function has zero arguments, it will
// be evaluated on the call First or Rest.
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

    void SetRest( VALUE rest ) {
        if (rest.IsHeapObject()) {
            rest.GetObject()->RefCount++;
        }
        _rest = rest;
    }
    
    void SetFirst( VALUE first ) {
        if (first.IsHeapObject()) {
            first.GetObject()->RefCount++;
        }
        _first = first;
    }


    Cons( VALUE first, VALUE rest ) : List() {
        SetFirst( first );
        SetRest( rest );
    }
    
    static Cons* Create( VALUE first, VALUE rest ) {
        Cons* c = MALLOC_HEAP(Cons);
        new (c) Cons(first,rest);  // Calling constructor
        c->CheckIntegrety();
        return c;
    }
    

    
    // Override of the List interface
    List* Append( VALUE elem ) {
#ifdef USE_CONS_OPTIMIZATIONS
        List* tail = this;
        while (!tail->Rest().IsNil() ) {
            tail = tail->Rest().GetList();
        }
        if (tail->RefCount < 2 && tail->Rest().IsNil()) {
            // We can optimize things as we can reuse this materalization.
            // Nobody is referencing it.
            if (tail->AttemptDirtyAdd(elem)) {
                return this;
            };
        }
#endif
        int originalCount = Count();
        int last = Count()-1;
        List* c = LIST(elem).GetList();
        while (last >= 0) {
            c = c->Prepend( GetAt(last) );
            last--;
        }
        int newCount = c->Count();
        assert( newCount == originalCount + 1 );
//        std::cout << "\nBefore:" << LIST(this).Print();
//        std::cout << "\nAdded:" << elem.Print();
//        std::cout << "\nAfter:" << LIST(c).Print() << "\n";
        return c;
    }
    
#ifdef USE_OPTIMIZATIONS
    bool AttemptDirtyAdd( VALUE v ) {
        if (RefCount > 1 || !_rest.IsNil()) {
            return false;        
        }
        LIST c;
        c.SetListPointer( (uintptr_t)Cons::Create( v, NIL() ) );
//        c.MaterializeAsCons( v, NIL() );
        SetRest(c);
        return true;
    }
#endif
    
    List* Prepend( VALUE v ) final {
        auto c = Cons::Create(v, LIST(this));
        //std::cout << "\nBefore:" << LIST(TList,this).Print();
        //std::cout << "\nPrepended:" << v.Print();
        //std::cout << "\nAfter:" << LIST(TList,c).Print() << "\n";
        return c;
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

    List* Take( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

    List* Last( int i = 1 ) final {
        throw std::runtime_error("Not implemented yet");
    }

};

}

}
#endif /* List_hpp */

#endif
