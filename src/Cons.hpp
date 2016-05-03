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





class Cons;



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
    
    
    LIST( Cons* cons ) {
        Integer = (uint64_t)cons;
        Flag = true; // See VALUE::IsClassicalParenthesis
    }
    
    LIST Append( VALUE elem );

    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    Cons* __allocateCons( VALUE car, VALUE cdr );
    
    Cons* GetCons() {
        return (Cons*)Integer;
    }
    
    
};


class List : public Object {
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



class Cons : public List {
public:
    VALUE Car;
    VALUE Cdr;
public:
    VALUE GetCar() {
        return Car;
    }
    
    VALUE GetCdr() {
        return Cdr;
    };
    
    Cons( VALUE _car, VALUE _cdr ) {
//        Type = CurrentIsolate->ConsType;
       // IsClassicalParenthesis = isParenthesis;
        Car = _car;
        Cdr = _cdr;
    }
    
    
    LIST Append( VALUE elem ) {
        if (RefCount==0) {
           if (GetCdr() != NIL()) {
               throw std::runtime_error("Can only append at the end of a list");
           }
           LIST c;
           auto x = c.__allocateCons( elem, NIL() );
           Cdr = c;
           return LIST(x);
        }
        throw std::runtime_error("Not implemented yet");
    }
};

#endif /* List_hpp */
