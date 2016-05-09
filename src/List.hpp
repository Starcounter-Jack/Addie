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
    LIST() {
        Type = TList;
        ListStyle = QParenthesis;
        Integer = 0;
    };
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
    
    
    virtual bool IsCheapCount() {
        return false;
    }
    
    virtual int Count() {
        int i = 1;
        VALUE list = this->Rest();
        while (!list.IsEmpty()) {
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
    virtual List* Take( int i ) = 0;
    
#ifdef USE_OPTIMIZATIONS
    virtual bool AttemptDirtyAdd( VALUE v ) {
        throw std::runtime_error("Not implemented yet");
    }
#endif
    
    List* RestL() {
        return Rest().GetList();
    }
    
};
     

    }
    
}


#endif /* List_hpp */
