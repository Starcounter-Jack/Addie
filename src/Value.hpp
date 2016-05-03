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
class List;



enum ValueStyle {
    QParenthesis = 0b00,
    QNumber = 0b00,
    QBrackets = 0b01,
    QBool = 0b01,
    QCurly = 0b10,
    QString = 0b11,
};

/** 
 * Most significat bit is means the object is allocated on the heap
 * Second most significant bit means this is a list
 */
enum ValueType : unsigned int {
    PNil              =  0b000,    // Lisp nil
    PSymbol           =  0b010,    // Symbol
    PNumber           =  0b011,    // Integer/Ratio
    PList             =  0b100,    // List/vector/array/string/map
    PLambda           =  0b101,    // Function/procedure/code
    PContinuation     =  0b110,    // Execution thread
    PStringOld        =  0b111     // TODO! REMOVE!
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

// Addie fits numbers, nil, booleans and pointers to larger objects in a fixed size
// structure that fits in a single 64 bit register on the CPU.
class VALUE {
public:
    
    union {
        uint64_t Whole;
        struct {
            // The materialized type
            ValueType Type : 3;
            // The percieved type. I.e. a percieved type can be a map while the
            // materialized type is a linked list. Another example is that the percieved
            // type is a string whereas the materialized type is a bitmapped vector.
            // Most list materialization can represent most percieved list type.
            ValueStyle Style : 2;
            // The actual data forming the numbers or pointers are stored here.
            int64_t Integer: 59;
        };
    };
    
    VALUE() : Whole(0) { // Everything is zero by default
    }
    
    // Lists and Lambdas obviously do not fit in a value.
    bool IsHeapObject() {
        return Type & 0b100;
    }
    
    uint8_t* OtherBytes();
    
//    bool operator==( const VALUE& rhs)
//    {
//        return Whole == rhs.Whole;
//    };
    
    // Values are the same if the bits are the same
//    bool operator!=(const VALUE& rhs)
//    {
//        return Whole != rhs.Whole;
//    };
    
    Object* GetObject() {
        return (Object*)Integer;
    }
    
    bool IsNil() {
        return Type == PNil;
    }
    
    bool IsList() {
        return Type == PList;
    }
    
    std::string ToString();

    
    std::string Print();
};



class STRINGOLD : public VALUE {
public:
    STRINGOLD(std::string str ) {
        Style = QString;
        AllocateString( str.c_str(), str.length() );
    }
    
    STRINGOLD(char* c, size_t size ) {
        Style = QString;
        AllocateString( c, size );
    }
    

    const char* c_str() {
        return (const char*)StringBytes();
    }

    uint32_t Length();
    
    uint8_t* StringBytes();

    std::string ToString();
    std::string Print();
    
    
    
    // TODO! Strings are immutable/persistent vectors.
    void AllocateString( const char* str, size_t size );
};

class NIL : public VALUE {
public:
    NIL() {
        Type = PNil;
    }
    
    std::string Print();
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
    std::string Print();
};

class INTEGER : public VALUE {
public:
    INTEGER() {
        Type = PNumber;
        Integer = 0;
    }
    
    INTEGER( uint64_t i ) {
        Type = PNumber;
        Integer = i;

    }
    
    std::string ToString();
    std::string Print();
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
    int RefCount = 0;
    List* Meta;
//    SYMBOL SetSpecifiers[32];
};

class Iterator {
    virtual bool hasNext() = 0;
    virtual VALUE next() = 0;
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





#endif /* Value_hpp */
