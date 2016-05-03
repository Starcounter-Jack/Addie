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


class List;



class LIST : public VALUE {
public:
    LIST() {
        Type = PList;
        Flag = true; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    LIST( VALUE _car, VALUE _cdr) {
        Type = PList;
        Flag = true; // See VALUE::IsClassicalParenthesis
        __allocateCons( _car, _cdr );
    }
    
    
    LIST( List* cons ) {
        Integer = (uint64_t)cons;
        Flag = true; // See VALUE::IsClassicalParenthesis
    }
    
    LIST Append( VALUE elem );
    
    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    Cons* __allocateCons( VALUE car, VALUE cdr );
    
    //Cons* GetCons() {
    //    return (Cons*)Integer;
    //}
    
    List* GetList() {
        return (List*)Integer;
    }
    
    
};


class List : public Object {
public:
    virtual VALUE GetCar() = 0;
    virtual VALUE GetCdr() = 0;
    virtual LIST Append( VALUE v ) = 0;
    /*    virtual VALUE GetAt( int i ) = 0;
     virtual VALUE SetAt( int i, VALUE v ) = 0;
     virtual VALUE SetCar(VALUE v) = 0;
     virtual VALUE SetCdr(VALUE v) = 0;
     virtual int Count() = 0;
     virtual VALUE Prepend( VALUE v );
     virtual VALUE RemoveAt( int i );
     virtual VALUE InsertAt( int i, VALUE v );
     virtual Iterator GetIterator() = 0;
     */
};



#endif /* List_hpp */
