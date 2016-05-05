//
//  List.hpp
//  Addie
//
//  Created by Joachim Wester on 05/05/16.
//
//

#ifndef List_hpp
#define List_hpp

#include "VM.hpp"

namespace Addie {
    namespace Internals {

// This value represents all kinds of lists (including strings and maps). If the list is empty,
// there will only be this value. If the list is non-empty, it will have a representation
// on the heap. The representation depends on the predicted use-case. I.e. if we suspect new
// lists will be derived, we will create a persistent vector. If not, we will create a simple
// array.
class LIST : public VALUE {
public:
    
    List* CreateDefaultList( VALUE first );
    List* CreateDefaultList( VALUE first, VALUE rest );
    
    
    LIST() {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    // Create a list that points to a Cons (a classical lisp linked list pair node)
    LIST( ValueStyle style, VALUE _first) {
        Type = PList;
        Style = style;
        SetPointer( (uintptr_t)CreateDefaultList( _first ) );
        CheckIntegrety();
    }
    
    /*
    // Create a list that points to a Cons (a classical lisp linked list pair node)
    LIST( ValueStyle style, VALUE _first, VALUE _rest) {
        Type = PList;
        Style = style; // See VALUE::IsClassicalParenthesis
        Integer = (uintptr_t)CreateDefaultList( _first, _rest );
        CheckIntegrety();
    }
     */
    
    VALUE Rest();
    
    
    void CheckIntegrety() {
        if (IsHeapObject()) {
            auto o = GetObject();
            o->CheckIntegrety();
        }
    }
    
    LIST( ValueStyle style, List* list ) {
        Type = PList;
        SetPointer( (uint64_t)list );
        Style = style;    }
    
    
    // Append to the end of this list. As this is slow in persistent linked lists,
    // the new list will probably have another type of materialization.
    LIST Append( VALUE elem );

    LIST Prepend( VALUE elem );
    
    
    // Empty lists are not allocated on the heap.
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    
    LIST ReplaceAt( int i, VALUE v );
    VALUE GetAt( int i );
    
    
};


class List : public Object {
public:
    
    List() : Object() {
        
    }
    
    virtual VALUE First() = 0;
    virtual VALUE Rest() = 0;
    virtual List* Append( VALUE v ) = 0;
    
    virtual List* Prepend( VALUE v ) = 0;
    
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

}
}


#endif /* List_hpp */
