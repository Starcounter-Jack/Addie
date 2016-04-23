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
        AllocateString( str );
    }
    
    
    std::string ToString();
    void Print();
    
    
    
    // TODO! Strings are immutable/persistent vectors.
    void AllocateString( std::string str ) {
        auto size = str.size();
        char* obj = (char*)Heap.SafeMalloc(size+1); // We add a zero termination so we have a
        // cheap conversion
        // to zero terminated strings if needed. TODO! GC
        void *original = &str[0];
        memcpy(obj, original, size);
        obj[size] = 0;
        this->Pointer = (uint64_t)obj;
    }
};

class NIL : public VALUE {
public:
    NIL() {
        IsHeapObject = false;
        PType = PNil;
    }
    
    void Print();
};



#endif /* Value_hpp */
