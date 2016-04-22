//
//  Value.hpp
//  Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#ifndef Value_hpp
#define Value_hpp

//#include <string>
#include <iostream>
#include <cstdint>
#include "Heap.hpp"



enum class Type : int {
    Bool     = 0b000,    // 1 bit integer
    Symbol   = 0b001,    // 8 bit or 32 bit symbol
    Integer  = 0b010,    // 60 bit integer
    Fraction = 0b011,    // 30 + 30 bit fraction
    Object   = 0b100,    // Heap allocated object
    Reserved1 = 0b101,   // Reserved Type
    Reserved2 = 0b110,   // Reserved Type
    Reserved3 = 0b111    // Reserved Type
};
   
// Let's make use of the fact that Starcounter only operates on modern operating systems.
// Let's pack real pointers together with primitive value representations that all fit
// in a single 64 register.
union Value {
    Value() : Whole(0) {
    }

    union {
        uint64_t Whole;
        void* Pointer;
        struct {
            union {
                long Integer: 60;
                struct {
                    int Numerator : 30;
                    int Denominator : 30;
                } Ratio;
            };
            int ReservedFlag : 1; // We have one spare bit for future use.
            Type Type : 3;    // This relies on that the OS does aligned allocations.
                          // The implementor needs to assert that the 4 least significant
                          // bits are zero on any allocation.
                          // For example, malloc on Windows always align such that 4 bits are free.
                          // http://www.codeproject.com/Articles/28818/bit-pointers-in-a-bit-world.
        } Primitive;
    };
    
    bool operator==( const Value& rhs)
    {
        return Whole == rhs.Whole;
    };
    
    bool operator!=(const Value& rhs)
    {
        return Whole != rhs.Whole;
    };
    
    // TODO! Strings are immutable/persistent vectors.
    static Value CreateString( std::string str ) {
        auto size = str.size();
        char* obj = (char*)Heap.SafeMalloc(size+1); // We add a zero termination so we have a
                                               // cheap conversion
                                               // to zero terminated strings if needed.
        void *original = &str[0];
        memcpy(obj, original, size);
        obj[size] = 0;
        Value v;
        v.Pointer = obj;
        return v;
    }
    std::string ToString() {
        if (Pointer == NULL) {
            return "NULL";
        }
        return (char*) Pointer;
    }
    
    void Print() {
        std::cout << "Printing \"";
        std::cout << ToString();
        std::cout << "\"";
    }
};
    


#endif /* Value_hpp */
