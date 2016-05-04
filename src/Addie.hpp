//
//  Addie.hpp
//  Addie
//
//  Created by Joachim Wester on 04/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Addie_hpp
#define Addie_hpp

#define USE_OPTIMIZATIONS

#ifdef USE_OPTIMIZATIONS
#define USE_CONS
#define USE_CONS_OPTIMIZATIONS
#define USE_INTARRAY
#define USE_INTARRAY_OPTIMIZATIONS
#define USE_ARRAY
#define USE_ARRAY_OPTIMIZATIONS
//#define USE_VECTOR
//#define USE_VECTOR_OPTIMIZATIONS
#endif



//#include "Isolate.hpp"
#include <iostream>
#include <assert.h>
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



enum ValueStyle : unsigned int  {
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
    
    List* GetList() {
        if (Type != PList ) {
            throw std::runtime_error("Not a list");
        }
        return (List*)Integer;
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
    
    bool IsInteger() {
        return Type == PNumber;
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
    
    INTEGER( int64_t i ) {
        Type = PNumber;
        Integer = i;
        assert( Integer == i );
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



enum Symbols {
    END,
    //    SET_REGISTER_WINDOW,
    EXIT_WITH_CONTINUATION,
    RETURN,
    MOVE,
    CALL_0,
    CALL_1,
    CALL_2,
    CALL_3,
    CALL_4,
    CALL_5,
    CALL_6,
    JMP,
    JMP_IF_TRUE,
    SymNil,
    SymFalse,
    SymTrue,
    SymString,
    SymPlus,
    SymMinus,
    SymStar,
    SymSlash,
    SymCons,
    SymPrint,
    Sym_Count
};
static const char *SymStrings[] = {
    "end",
    //    "set-register-window",
    "exit-with-continuation",
    "return",
    "move",
    "call-0",
    "call-1",
    "call-2",
    "call-3",
    "call-4",
    "call-5",
    "call-6",
    "jmp",
    "jmp-if-true",
    "nil",
    "false",
    "true",
    "string",
    "+",
    "-",
    "*",
    "/",
    "Cons",
    "print"
};


// Contains fixed registers (used for variables instead of stack)
// Registers are preloaded with constants
//class Frame : Object {
//public:
//    VALUE Variants[10]; // Variant variables/registers
//    VALUE Invariants[10]; // Invariant variables/registers and constants
//};

typedef uint8_t Op;

// Represents a byte code instruction. Addie bytecodes are dispatched in 32 bit blocks (instruction words). You can read more about it [here](https://github.com/Starcounter-Jack/Addie/wiki/Byte-Code)
class Instruction {
public:
    union {
        uint32_t Whole;
        int32_t A3 : 24;
        struct {
            int8_t A;
            int8_t B;
            int8_t C;
            Op OP;
        };
    };
    
    Instruction( Op op ) {
        OP = op;
    }
    
    Instruction( Op op, int16_t a2, byte c  ) {
        OP = op;
        A = a2 & 0x00FF;
        B = a2 & 0xFF00;
        C = c;
        assert( A2() == a2 );
        assert( C == c );
    }
    
    Instruction( Op op, int a3  ) {
        A3 = a3;
        OP = op;
        assert( A3 == a3 );
    }
    
    Instruction( Op op, int a, int b, int c ) {
        OP = op;
        A = a;
        B = b;
        C = c;
        assert( A == a );
        assert( B == b );
        assert( C == c );
    }
    Instruction( Op op, byte a, byte b ) {
        OP = op;
        A = a;
        B = b;
        assert( A == a );
        assert( B == b );
    }
    Instruction( Op op, byte a ) {
        OP = op;
        A = a;
        assert( A == a );
    }
    
    int32_t A2() {
        return A + (B<<8);
    }
    
};



class OpCall : public Instruction {
public:
    OpCall( byte symreg, byte p1reg, byte p2reg ) : Instruction( CALL_2, symreg, p1reg, p2reg ) {     }
    OpCall( byte symreg, byte p1reg ) : Instruction( CALL_1, symreg, p1reg ) {
    }
    OpCall( byte symreg ) : Instruction( CALL_0, symreg )   { }
};
class OpMove : public Instruction {
public:
    OpMove( byte reg1, byte reg2 ) : Instruction( MOVE, reg1, reg2 ) {
    }
};








#endif /* Addie_hpp */


