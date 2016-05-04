//
//  List.hpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef List_hpp
#define List_hpp

#include "Addie.hpp"
#include "Isolate.hpp"

class List;
class Cons;
class Array;

// This value represents all kinds of lists (including strings and maps). If the list is empty,
// there will only be this value. If the list is non-empty, it will have a representation
// on the heap. The representation depends on the predicted use-case. I.e. if we suspect new
// lists will be derived, we will create a persistent vector. If not, we will create a simple
// array.
class LIST : public VALUE {
public:
    LIST() {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
#ifdef USE_CONS
    // Create a list that points to a Cons (a classical lisp linked list pair node)
    LIST( VALUE _first, VALUE _rest) {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        MaterializeAsCons( _first, _rest );
    }
#endif
    
    VALUE Rest();
    
    
    LIST( ValueStyle style, List* list ) {
        Type = PList;
        Integer = (uint64_t)list;
        Style = style;    }
    
    
    // Append to the end of this list. As this is slow in persistent linked lists,
    // the new list will probably have another type of materialization.
    LIST Append( VALUE elem );
    
    
    // Empty lists are not allocated on the heap.
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
#ifdef USE_CONS
    Cons* MaterializeAsCons( VALUE first, VALUE rest );
#endif
//#ifdef USE_ARRAY
//    Array* MaterializeAsArray( VALUE first, VALUE rest );
//#endif
    
    LIST ReplaceAt( int i, VALUE v );
    VALUE GetAt( int i );
    
    
};


class List : public Object {
public:

    
    virtual VALUE First() = 0;
    virtual VALUE Rest() = 0;
    virtual List* Append( VALUE v ) = 0;
    
#ifdef USE_CONS
    virtual List* Prepend( VALUE v );
#else
    virtual List* Prepend( VALUE v ) = 0;
#endif
    
    virtual List* Skip( int index ) {
        List* list = this;
        for (int i = 0 ; i < index ; i++ ) {
            list = list->Rest().GetList();
        }
        return list;
    }

    virtual VALUE GetAt( int index ) {
        return Skip(index)->First();
    }
    
    virtual List* ReplaceAt( int i, VALUE v ) = 0;
    
    virtual int Count() {
        int i = 1;
        VALUE list = this->Rest();
        while (!list.IsNil()) {
            i++;
            list = list.GetList()->Rest();
        }
        return i;
    }
    
    virtual List* Last( int n ) {
        int cnt = Count();
        return Skip(cnt-n);
    }

    virtual List* RemoveAt( int i ) = 0;
    virtual List* InsertAt( int i, VALUE v ) = 0;
    virtual List* Reverse() = 0;
    virtual List* Replace( VALUE v1, VALUE v2 ) = 0;
    virtual List* Sort( VALUE fun ) = 0;
    virtual List* Map( VALUE fun ) = 0;
    virtual List* First( int i ) = 0;
    
#ifdef USE_OPTIMIZATIONS
    virtual bool AttemptDirtyAdd( VALUE v ) {
        throw std::runtime_error("Not implemented yet");
    }
#endif
    
    bool AtEnd() {
        return Rest().IsNil();
    }
    
    List* RestL() {
        return Rest().GetList();
    }

};



#endif /* List_hpp */
