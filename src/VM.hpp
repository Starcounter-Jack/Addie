//
//  Continuation.hpp
//  Addie
//
//  Created by Joachim Wester on 28/04/16.
//
//

#ifndef VM_hpp
#define VM_hpp

#include "Isolate.hpp"
#include "Vector.hpp"
#include "Value.hpp"
#include <assert.h>

// Contains fixed registers (used for variables instead of stack)
// Registers are preloaded with constants
//class Frame : Object {
//public:
//    VALUE Variants[10]; // Variant variables/registers
//    VALUE Invariants[10]; // Invariant variables/registers and constants
//};

typedef uint8_t Op;


class Instruction {
public:
    union {
        int32_t Whole;
        int32_t A3 : 24;
        struct {
        char A;
        char B;
        char C;
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
    }
    
    Instruction( Op op, int32_t a3  ) {
        A3 = a3;
        OP = op;
    }
    
    Instruction( Op op,byte a, byte b, byte c ) {
        OP = op;
        A = a;
        B = b;
        C = c;
    }
    Instruction( Op op, byte a, byte b ) {
        OP = op;
        A = a;
        B = b;
    }
    Instruction( Op op, byte a ) {
        OP = op;
        A = a;
    }
    
    int32_t A2() {
        return A + (B<<8);
    }

};

class OpLoadConst : public Instruction {
public:
    OpLoadConst( byte regno, int16_t value ) : Instruction( LOAD_CONST,value,regno) {
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
// As such, frames are objects that needs to be garbage collected.
// You can view frames as a spaghetti stack where each node is the
// exact size needed by a function. The frame is like a stack with direct addressing rather
// than pushing and popping.
class Frame : public Object {
public:
    Frame* Parent;
    Compilation* Comp;
    // VALUE v1; VALUE v2....
    
    VALUE* GetStartOfRegisters() {
        return (VALUE*)(((byte*)this) + sizeof(Frame));
    }
};


// Continuations are used by value as it is only 16 bytes in size.
class Continuation {
public:
    Instruction* PC;                    // Program Counter (aka Instruction Pointer).
    Frame* frame = NULL;
    
    void EnterIntoNewFrame( Compilation* code, Frame* parent ) {
        assert( frame == NULL );
        // The compiled code contains the size of the register machine needed for the
        // code. It also contains the initial values for the registers that are either
        // invariant or that have a initial value.
        frame = (Frame*)CurrentIsolate->MallocHeap(sizeof(Frame) + code->SizeOfRegisters);
        new (frame) Frame();
        //Registers = (VALUE*)(((byte*)frame) + sizeof(Frame));
        memcpy( ((byte*)frame) + sizeof(Frame), ((byte*)code) + sizeof(Compilation), code->SizeOfInitializedRegisters );
        frame->Comp = code;
        frame->Parent = parent;
    }
};


#endif /* VM_hpp */