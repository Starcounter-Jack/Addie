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

class CONS : public VALUE {
public:
    CONS() {
        Type = TCons;
        Integer = 0;
    };

    CONS( VALUE _car, VALUE _cdr) {
        Type = TCons;
        __allocateCons( _car, _cdr );
    }
    
    
    CONS( Cons* cons ) {
        Integer = (uint64_t)cons;
    }
    
    // Snoc is the reverse of Cons. Stolen from Emacs terminology. Bang! is because
    // this should only be done on mutable lists.
    CONS SnocBANG( VALUE elem );

    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    void Print();
    
    Cons* GetCons() {
        return (class Cons*)Integer;
    }
    
private:
    Cons* __allocateCons( VALUE car, VALUE cdr );
    
    
    
};


class Cons : public Object {
public:
    VALUE Car;   // Aka CAR
    VALUE Cdr;    // Aka CDR

    Cons( VALUE _car, VALUE _cdr ) {
//        Type = CurrentIsolate->ConsType;
        Car = _car;
        Cdr = _cdr;
    }
    
    

};



#endif /* List_hpp */
