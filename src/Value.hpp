//
//  Value.hpp
//  Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Value_hpp
#define Value_hpp

//#include <string>
#include <iostream>


#include <cstdint>

typedef uint8_t byte;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uint16_t Symbol;


// * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));


class Object;
class Isolate;
class Continuation;

enum ValueType : int {
    PNil        =  0b0000,    // 1 bit integer
    PBool       =  0b0001,    // 1 bit integer
    PSymbol     =  0b0010,    // Symbol
    PInteger    =  0b0011,    // 60 bit signed integer
    PFloat      =  0b0100,    // 60 bit signed float????
    PFraction   =  0b0101,
    PReserved1   =  0b0110,
    PReserved2   =  0b0111,
    TVector     =  0b1000,
    TString     =  0b1001,
    TCons       =  0b1010,  // List
    TLambda     =  0b1011,
};

/*
class VALUE8 {
    byte OpCode : 8;
};

class VALUE16 {
    byte OpCode : 8;
    union {
        char Char;
        int8_t Integer;
    };
};

class VALUE32 {
    byte OpCode : 8;
    union {
        uint16_t Char;
        int16_t Integer;
    };
};
 */

// Let's make use of the fact that Starcounter only operates on modern operating systems.
// Let's pack real pointers together with primitive value representations that all fit
// in a single 64 register.
class VALUE {
public:
    
    union {
        uint64_t Whole;
        struct {
            uint8_t Type : 4;
            int64_t Integer: 60;
        };
    };
    
    VALUE() : Whole(0) { // Everything is zero by default
    }
    
    bool IsHeapObject() {
        return Type & 0b1000;
    }

    uint8_t* OtherBytes();
    
    bool operator==( const VALUE& rhs)
    {
        return Whole == rhs.Whole;
    };
    
    bool operator!=(const VALUE& rhs)
    {
        return Whole != rhs.Whole;
    };
    
    Object* GetObject() {
        return (Object*)Integer;
    }
    
    bool IsNil() {
        return Type == PNil;
    }
    
    bool IsCons() {
        return Type == TCons;
    }

    
    void Print();
};



class STRING : public VALUE {
public:
    STRING(std::string str ) {
        Type = TString;
        AllocateString( str.c_str(), str.length() );
    }
    
    STRING(char* c, size_t size ) {
        Type = TString;
        AllocateString( c, size );
    }
    

    const char* c_str() {
        return (const char*)StringBytes();
    }

    uint32_t Length();
    
    uint8_t* StringBytes();

    std::string ToString();
    void Print();
    
    
    
    // TODO! Strings are immutable/persistent vectors.
    void AllocateString( const char* str, size_t size );
};

class NIL : public VALUE {
public:
    NIL() {
        Type = PNil;
    }
    
    void Print();
};

class SYMBOL : public VALUE {
public:
    SYMBOL() {
        Type = PSymbol;
    }
    SYMBOL( uint32_t sym ) {
        Type = PSymbol;
        Integer = sym;
    }
    
    SYMBOL( const char* str, size_t len );
    
    std::string ToString();
    void Print();
};

class INTEGER : public VALUE {
public:
    INTEGER() {
        Type = PInteger;
        Integer = 0;
    }
    
    INTEGER( int i ) {
        Type = PInteger;
        Integer = i;

    }
    
    std::string ToString();
    void Print();
};

class CONTINUATION : public VALUE {
public:
    Continuation* GetContinuation() {
        return (Continuation*)OtherBytes();
    }
    
};


class Type;


// The root heap object
class Object {
public:
    int RefCount = 1;
//    SYMBOL SetSpecifiers[32];
};


class NamedEntity : public Object {
public:
    Symbol Name;
};

// The root type object
class Type : public NamedEntity {
public:
    Type( Symbol symbol ) {
        Name = symbol;
    }
};

//#define PUSH_INTEGER(x) INTEGER(x)
//#define CALL(x,y) SYMBOL(x,y)
//#define CALL(x) SYMBOL(x)





#endif /* Value_hpp */
