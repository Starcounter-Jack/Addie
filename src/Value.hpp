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
#include "Heap.hpp"

class Isolate;

enum PrimitiveType : int {
    PNil        =  0b000,    // 1 bit integer
    PBool       =  0b001,    // 1 bit integer
    PSymbol     =  0b010,    // 8 bit or 32 bit symbol
    PInteger    =  0b011,    // 60 bit signed integer
    PFloat      =  0b100,    // 60 bit signed float????
    PFraction   =  0b101,
    PReserved1   =  0b110,
    PReserved2   =  0b111
};
enum HeapType : int {
    TVector     =  0b000,
    TString     =  0b001,
    TCons       =  0b010,  // List
    TLambda     =  0b011,
};



// Let's make use of the fact that Starcounter only operates on modern operating systems.
// Let's pack real pointers together with primitive value representations that all fit
// in a single 64 register.
class VALUE {
public:
    int foo;
    
union  {

    union {
        uint64_t Whole;
        struct {
            bool IsHeapObject : 1;
            union {
                long Integer: 60;
                uint64_t Pointer: 60;
                struct {
                    int Numerator : 30;
                    int Denominator : 30;
                } Ratio;
            };
            union {
                union {
                    PrimitiveType PType : 3;
                    HeapType HType : 3;
                };
            };
                          // This relies on that the OS does aligned allocations.
                          // The implementor needs to assert that the 4 least significant
                          // bits are zero on any allocation.
                          // For example, malloc on Windows always align such that 4 bits are free.
                          // http://www.codeproject.com/Articles/28818/bit-pointers-in-a-bit-world.
        };
    };
};
    
    VALUE() : Whole(0) { // Everything is zero by default
    }

    
    bool operator==( const VALUE& rhs)
    {
        return Whole == rhs.Whole;
    };
    
    bool operator!=(const VALUE& rhs)
    {
        return Whole != rhs.Whole;
    };
    
    void Print();
};



class STRING : public VALUE {
public:
    STRING(std::string str ) {
        IsHeapObject = true;
        HType = TString;
        AllocateString( str.c_str(), str.length() );
    }
    
    STRING(char* c, size_t size ) {
        IsHeapObject = true;
        HType = TString;
        AllocateString( c, size );
    }
    
    uint8_t* Bytes() {
        return (uint8_t*)Pointer;
    }
    
    
    std::string ToString();
    void Print();
    
    
    
    // TODO! Strings are immutable/persistent vectors.
    void AllocateString( const char* str, size_t size );
};

class NIL : public VALUE {
public:
    NIL() {
        IsHeapObject = false;
        PType = PNil;
    }
    
    void Print();
};

class SYMBOL : public VALUE {
public:
    SYMBOL() {
        IsHeapObject = false;
        PType = PSymbol;
    }
    
    SYMBOL( const char* str, size_t len );
    
    std::string ToString();
    void Print();
};

class INTEGER : public VALUE {
public:
    INTEGER() {
        IsHeapObject = false;
        PType = PInteger;
        Integer = 0;
    }
    
    INTEGER( int i ) {
        IsHeapObject = false;
        PType = PInteger;
        Integer = i;

    }
    
    std::string ToString();
    void Print();
};





#endif /* Value_hpp */
