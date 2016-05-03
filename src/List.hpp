//
//  List.hpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//
//

#ifndef List_hpp
#define List_hpp


#include "Value.hpp"
#include "Isolate.hpp"

class List;



class LIST : public VALUE {
public:
    LIST() {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    LIST( VALUE _first, VALUE _rest) {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        MaterializeAsCons( _first, _rest );
    }
    
    
    LIST( List* cons ) {
        Integer = (uint64_t)cons;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
    }
    
        
    LIST Append( VALUE elem );
    
    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    Cons* MaterializeAsCons( VALUE first, VALUE rest );

    void BeginWriteArray() {
        Integer = CurrentIsolate->NextOnHeap;
        *((int*)CurrentIsolate->NextOnHeap) = 0;
        CurrentIsolate->NextOnHeap += sizeof(int);
    }
    
    void ArrayWrite( VALUE v ) {
        (*((VALUE*)CurrentIsolate->NextOnHeap)) = v;
        CurrentIsolate->NextOnHeap += sizeof(VALUE);
        (*((int*)Integer))++;
    }
    
    LIST SetAt( int i, VALUE v );    
    VALUE GetAt( int i );
    
    List* GetList() {
        return (List*)Integer;
    }
    
    
};


class List : public Object {
public:
    virtual VALUE First() = 0;
    virtual VALUE Rest() = 0;
    
    virtual LIST Append( VALUE v ) = 0;
    virtual LIST Prepend( VALUE v ) = 0;
    
    virtual VALUE GetAt( int i ) = 0;
    virtual LIST SetAt( int i, VALUE v ) = 0;
    
    virtual int Count() = 0;
    /*
     virtual VALUE RemoveAt( int i );
     virtual VALUE InsertAt( int i, VALUE v );
     virtual VALUE Concatenate( VALUE v );
     virtual VALUE Reverse();
     virtual VALUE Replace( VALUE v1, VALUE v2 );
     virtual VALUE Sort( VALUE fun );
     virtual VALUE Map( VALUE fun );
     virtual VALUE First( int i = 1 );
     virtual VALUE Last( int i = 1 );
     virtual VALUE Skip( int i );
     */
};



#endif /* List_hpp */
