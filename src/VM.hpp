//
//  VM.hpp
//  Addie
//
//  Created by Joachim Wester on 28/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef VM_hpp
#define VM_hpp

#include "Addie.hpp"
#include "Isolate.hpp"
#include "Optimized_Vector.hpp"
#include <assert.h>

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





//class Compilation {
//public:
//    Instruction* Code;
//    VALUE* Registers;
//};


struct Compilation {
    u32 SizeOfRegisters;
    u32 SizeOfInitializedRegisters;
    
    
    VALUE* StartOfConstants() {
        return (VALUE*)((byte*)this + sizeof(Compilation));
    }
    
    Instruction* StartOfInstructions() {
        return (Instruction*)((byte*)this + sizeof(Compilation) + SizeOfInitializedRegisters);
    }
    
    int GetInitializedRegisterCount() {
        return SizeOfInitializedRegisters / sizeof(VALUE);
    }
    
    //Instruction* Code;
    // VALUE v1; VALUE v2....
};

// Frames are register windows pertaining to a particular compilation.
// Here the actual register values are kept.
// As frames are referenced by continuations. The user can create many continuation objects.
// Each frame is linked to its parent frame (it can be likened with a spaghetti stack).
// You can view frames as a spaghetti stack where each node is the
// exact size needed by a function. The frame is like a stack with direct addressing rather
// than pushing and popping.
// Frames are NOT using the garbage collector as frames can more cheaply allocated and
// deallocated.
struct Frame
{
    Frame* Parent;
    Compilation* Comp;
    // VALUE Registers[n];
    
    VALUE* GetStartOfRegisters() {
        return (VALUE*)(((byte*)this) + sizeof(Frame));
    }
};


// Continuations are used by value as it is only 16 bytes in size (on 64 bits architectures).
class Continuation {
public:
    Instruction* PC;                    // Program Counter (aka Instruction Pointer).
    Frame* frame = NULL;
    
    void EnterIntoNewFrame( Compilation* code, Frame* parent ) {
        assert( frame == NULL );
        // The compiled code contains the size of the register machine needed for the
        // code. It also contains the initial values for the registers that are either
        // invariant or that have a initial value.
        frame = (Frame*)CurrentIsolate->AdvanceStack(sizeof(Frame) + code->SizeOfRegisters);
        memcpy( ((byte*)frame) + sizeof(Frame), ((byte*)code) + sizeof(Compilation), code->SizeOfInitializedRegisters );
        frame->Comp = code;
        frame->Parent = parent;
    }
};

class Compiler {
public:
    static Compilation* Compile( VALUE form );
    static Compilation* CompilePrototype( VALUE form );
    static STRINGOLD Disassemble( Compilation* code );
};


class Interpreter {
public:
    
    static Continuation Interpret( Continuation cont );
    
    
    static Continuation Interpret( Compilation* code ) {
        Continuation c;
        c.EnterIntoNewFrame(code, NULL);
        c.PC = code->StartOfInstructions();
        return Interpreter::Interpret( c );
    }
    
};


#endif /* VM_hpp */