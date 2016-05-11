//
//  Addie.hpp
//  Addie
//
//  Created by Joachim Wester on 04/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Addie_hpp
#define Addie_hpp

//#define USE_OPTIMIZATIONS

#ifdef USE_OPTIMIZATIONS
#define USE_CONS
#define USE_CONS_OPTIMIZATIONS
#define USE_ARRAY
#define USE_ARRAY_OPTIMIZATIONS
#define USE_INTARRAY
#define USE_INTARRAY_OPTIMIZATIONS
//#define USE_VECTOR
//#define USE_VECTOR_OPTIMIZATIONS
#else
#define USE_CONS
#endif



//#include "Isolate.hpp"
#include <iostream>
#include <assert.h>
#include <cstdint>
#include <map>
#include <vector>
#include <sys/mman.h>

namespace Addie {
namespace Internals {
    
typedef uint8_t byte;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uint16_t Symbol;


// * c = (Cons*)CurrentIsolate->Memory.MallocHeap(sizeof(Cons));

    
    

class List;

// The root heap object
class Object {
public:
    uint32_t MemoryCheck = 123456789; // 0xABBACAFFE;
    int RefCount = 0;
    List* Meta;
    //    SYMBOL SetSpecifiers[32];
    
    virtual void CheckIntegrety() {
        if (MemoryCheck != 123456789 ) { // 0xABBACAFFE ) {
            throw std::runtime_error("Memory is compromised");
        }
    }

};





enum ValueType : uint8_t {
    TAtom      =  0b00,    // Symbol
    TNumber    =  0b01,    // Integer
    TOther     =  0b10,    // Function/procedure/code
    TList      =  0b11,    // List/vector/array/string/map
};

    
enum ValueListStyle : uint8_t {
    QParenthesis    =  0b00,    //
    QString         =  0b01,    //
    QBracket        =  0b10,    // List/vector/array/string/map
    QCurly          =  0b11,    // List/vector/array/string/map
};
    
enum ValueNumberSubType : uint8_t {
    NInteger    =  0b00,    //
    NFraction   =  0b01,    //
    NBool       =  0b10,    //
    NReserved   =  0b11,    //
};
    
enum ValueAtomSubType : uint8_t {
    ANil        =  0b00,    //
    ASymbol     =  0b01,    //
    AKeyword    =  0b10,    //
    AOther      =  0b11,    //
};
    
enum ValueOtherSubType : uint8_t {
     OFunction    =  0b00,    //
     OValueReference =  0b01,    // For future imperative closures
     OOldString   =  0b10,    //
     OOther       =  0b11,    //
};
    



// Addie fits numbers, nil, booleans and pointers to larger objects in a fixed size
// structure that fits in a single 64 bit register on the CPU.
class VALUE {
public:
    
    union {
        uint64_t Whole;
        struct {
            ValueType Type : 2;
            ValueOtherSubType OtherSubType : 2;
            int64_t OtherPointer: 60;
        };
        struct {
            ValueType Type_ : 2;
            ValueAtomSubType AtomSubType : 2;
            int64_t Integer: 60;
        };
        struct {
            ValueType Type__ : 2;
            ValueAtomSubType AtomSubType_ : 2;
            uint32_t Notused: 28;
            uint32_t SymbolId: 32;
        };
        struct {
            ValueType Type___ : 2;
            ValueNumberSubType NumberSubType : 2;
            uint32_t Denominator: 28;
            int32_t Numerator: 32;
        };
        struct {
            ValueType Type____ : 2;
            ValueListStyle ListStyle : 2;
            uint32_t Start : 28;
            uintptr_t ListPointer : 32;
        };

    };
    
    /*
    inline bool IsConstant() {
        switch (Type) {
            case TList:
                if (ListStyle == QString) {
                    return true;
                }
                break;
            case TAtom:
                if ( AtomSubType == ANil && AtomSubType == AKeyword ) {
                    return true;
                }
                break;
            case TNumber:
                return true;
            case TOther:
                return false;
        }
        return false;
    }
     */
    
    List* CreateDefaultList( VALUE first );
    List* CreateDefaultList( VALUE first, VALUE rest );
    
    
    VALUE() : Whole(0) { // Everything is zero by default
    }
    
    int Count();
    
    
    VALUE( ValueListStyle style) {
        Type = TList;
        ListStyle = style;
        Integer = 0;
    }
    
    // Create a list that points to a Cons (a classical lisp linked list pair node)
    VALUE( ValueListStyle style, VALUE _first) {
        Type = TList;
        ListStyle = style;
        SetListPointer( (uintptr_t)CreateDefaultList( _first ) );
        CheckIntegrety();
    }
    
    
    //    LIST Assoc( VALUE key, VALUE value );
    
    
    /*
     // Create a list that points to a Cons (a classical lisp linked list pair node)
     LIST( ListTyle style, VALUE _first, VALUE _rest) {
     Type = PList;
     Style = style; // See VALUE::IsClassicalParenthesis
     Integer = (uintptr_t)CreateDefaultList( _first, _rest );
     CheckIntegrety();
     }
     */
    
    
    
    
    VALUE( ValueListStyle style, List* list ) {
        Type = TList;
        ListStyle = style;
        SetListPointer( (uint64_t)list );
    }
    
    VALUE( List* list ) {
        Type = TList;
        ListStyle = QParenthesis;
        SetListPointer( (uint64_t)list );
    }
    
    
    
    
    // Empty lists are not allocated on the heap.
    bool IsEmptyList() {
        assert( Type == TList );
        return (Type == TList && Integer == 0);
    }
    
    std::string Print();
    std::string PrintList();
  
    
    VALUE ReplaceAt( int i, VALUE v );
    VALUE GetAt( int i );
    
    
    

    // Lists and Lambdas obviously do not fit in a value.
    bool IsHeapObject() {
        return ( Type & 0b10 ) && Integer != 0;
    }
    
    VALUE Rest();
    VALUE First();

    // Append to the end of this list. As this is slow in persistent linked lists,
    // the new list will probably have another type of materialization.
    VALUE Append( VALUE elem );
    
    VALUE Prepend( VALUE elem );
    
    List* GetList() {
        assert( IsList() );
        return (List*)GetObject();
    }
    
    inline void SetListPointer( uintptr_t p ) {
        ListPointer = p >> 4;
        assert( (uintptr_t)GetListPointer() == p );
    }
    
    inline Object* GetListPointer() {
        uintptr_t p = ListPointer << 4;
        p = p | 0xcaf0000000;
        return (Object*)p;
    }
    
    Object* GetObject() {
        if (!IsHeapObject()) {
            throw std::runtime_error("Not a heap object");
        }
        Object* o;
        if (IsList()) {
            o = GetListPointer();
        } else {
            o = (Object*)OtherPointer;
        }
        o->CheckIntegrety();
        return o;
    }
    
    void CheckIntegrety() {
        if (IsHeapObject()) {
            auto o = GetObject();
            o->CheckIntegrety();
        }
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

    
    bool IsNil() {
        return Whole == 0;
    }
    
    
    bool IsEmpty() {
        return Integer == 0;
    }

    
    bool IsList() {
        //return ( Tag & 0b1100 ) == 0b1100;
        return Type == TList;
    }
    
    bool IsSymbol() {
        //return ( Tag & 0b1100 ) == 0b1100;
        return Type == TAtom && AtomSubType == ASymbol;
    }

    
//    inline bool EvaluatesToSelf() {
//        return Type != TList || ListStyle != QParenthesis;
//    }
    
    bool IsInteger() {
        return Type == TNumber && NumberSubType == NInteger;
    }
    
    std::string ToString();
    
//    std::string Print();
};



class STRINGOLD : public VALUE {
public:
    STRINGOLD(std::string str ) {
        Type = TOther;
        OtherSubType = OOldString;
        AllocateString( str.c_str(), str.length() );
    }
    
    STRINGOLD(char* c, size_t size ) {
        Type = TOther;
        OtherSubType = OOldString;
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
        Whole = 0; // = TagNil;
    }
    
    std::string Print();
};

class SYMBOL : public VALUE {
public:
    SYMBOL() {
        Type = TAtom;
        AtomSubType = ASymbol;
    }
    SYMBOL( uint32_t sym ) {
        Type = TAtom;
        AtomSubType = ASymbol;
        SymbolId = sym;
    }
    
    SYMBOL( const char* str, size_t len );
    
    std::string ToString();
    std::string Print();
};

    
class INTEGER : public VALUE {
public:
    INTEGER() {
        Type = TNumber;
        NumberSubType = NInteger;
        Integer = 0;
    }
    
    INTEGER( int64_t i ) {
        Type = TNumber;
        NumberSubType = NInteger;
        Integer = i;
        assert( Integer == i );
    }
    
    std::string ToString();
    std::string Print();
};
    
class Continuation;


class CONTINUATION : public VALUE {
public:
    Continuation* GetContinuation() {
        return (Continuation*)OtherBytes();
    }
    
};


//class Type;




class NamedEntity : public Object {
public:
    Symbol Name;
};

//// The root type object
//class Type : public NamedEntity {
//public:
//    Type( Symbol symbol ) {
//        Name = symbol;
//    }
//};



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
    SymLetStar,
    SymDef,
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
    "let*",
    "def",
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
        A3 = 0;
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


//#include "Addie.hpp"
//#include <cstring>


#define MALLOC_HEAP(type) (type*)CurrentIsolate->MallocHeap(sizeof(type));
#define MALLOC_HEAPS(size) CurrentIsolate->MallocHeap(size);



//class Variable : public NamedEntity {
//};

class Namespace : public NamedEntity {
public:
    std::map<Symbol,uint32_t> SlotBindings;
    std::vector<VALUE> Slots;
};


class Function : public NamedEntity {
};

class Continuation;
    struct Compilation;

// To allow multiple VMs in the same process.
class Isolate {
    
public:
    std::map<std::string,Symbol> SymbolsIds;  // { firstname:1, lastname:2, foo:3, bar:4 }
    std::vector<std::string> SymbolStrings;     // [ "firstname", "lastname", "foo", "bar" ]
    std::map<Symbol,Namespace*> Namespaces;
    std::map<Symbol,Compilation*> BuiltInFunctions;

    
    uintptr_t Stack;
    uintptr_t Constants;
    uintptr_t Heap;
    
    uintptr_t NextOnStack;
    uintptr_t NextOnHeap;
    uintptr_t NextOnConstant;
    
    
    int NumberOfAllocations = 0;
    int BytesAllocated = 0;
    
    
    uintptr_t ReserveMemoryBlock(uint64_t address, size_t size) {
        uintptr_t addr = address;
        
        addr = (uintptr_t)mmap((void *) addr, size,
                    PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        //printf("%p\n", addr);
        if((void*)addr == MAP_FAILED)
            throw std::runtime_error("Addie is trying to reserving 3TB of fixed memory addresses and fails. Jack was being a little bit too optimisitic.");
        return addr;
    }
    
    
    void CheckAddress(uintptr_t addr) {
        //uint64_t i = (uint64_t)addr;
        if ( addr & 0b0000000000000000000000000000000000000000000000000000000000001111 ) {
            throw std::runtime_error( "Addresses must be aligned" );
        }
    }
    
    inline void AlignNextHeap() {
        NextOnHeap = (NextOnHeap + 0b01111) & ~0b01111; // Round up for alignment
    }
    
    inline void AlignNextConstant() {
        NextOnConstant = (NextOnConstant + 0b01111) & ~0b01111; // Round up for alignment
    }
    
    
    uintptr_t MallocConstant( size_t size ) {
        uintptr_t newAddress = NextOnConstant;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnConstant += size;
        AlignNextConstant();
        CheckAddress(newAddress);
        return newAddress;
    }
    
    
    //    void* MallocCode( size_t size ) {
    //        void* newAddress = (void*)NextOnCode;
    //        BytesAllocated += size;
    //        NumberOfAllocations++;
    //        NextOnCode += size;
    //        NextOnCode = (NextOnCode + 0b01111) & ~0b01111; // Round up for alignment
    //        CheckAddress(newAddress);
    //        return newAddress;
    //    }
    
    
    uintptr_t AdvanceStack( size_t size ) {
        uintptr_t newAddress = (uintptr_t)NextOnStack;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnStack += size;
        NextOnStack = (NextOnStack + 0b01111) & ~0b01111; // Round up for alignment
        CheckAddress(newAddress);
        return newAddress;
    }
    
    inline void ReportConstantWrite( uintptr_t addr ) {
        NextOnConstant = addr;
        AlignNextConstant();
    }
    
    inline void ReportHeapWrite( size_t size ) {
        NextOnHeap += size;
        AlignNextHeap();
    }
    
    
    uintptr_t MallocHeap( size_t size ) {
        uintptr_t newAddress = NextOnHeap;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnHeap += size;
        //        NextOnHeap = (NextOnHeap + 0b01111) & ~0b01111; // Round up for alignment
        AlignNextHeap();
        CheckAddress(newAddress);
        return newAddress;
    }
    
    void PrintStatus();
    
    
    
//    Type* StringType;
//    Type* ConsType;
    
    
public: Isolate();
    
    
    Continuation* Root;
    
    VALUE EmptyList;
    NIL Nil;
    
    uint LastSymbolUsed = 0;
    
    std::string GetStringFromSymbolId( uint32_t id ) {
        return SymbolStrings[id];
    }
    
    Symbol RegisterNamespace( Symbol sym ) {
        
        auto x = Namespaces.find(sym);
        
        if (x != Namespaces.end())
            throw std::runtime_error("Namespace does already exist");
        
        Namespaces[sym] = new Namespace(); // TODO! GC
        return sym;
    }
    
    Symbol RegisterSymbol( const char* str, size_t size, int known );
};

// http://stackoverflow.com/questions/23791060/c-thread-local-storage-clang-503-0-40-mac-osx
#if HAS_CXX11_THREAD_LOCAL
#define ATTRIBUTE_TLS thread_local
#elif defined (__GNUC__)
#define ATTRIBUTE_TLS __thread
#elif defined (_MSC_VER)
#define ATTRIBUTE_TLS __declspec(thread)
#else // !C++11 && !__GNUC__ && !_MSC_VER
#error "Define a thread local storage qualifier for your compiler/platform!"
#endif

    
    


class String : public Object {
public:
    uint32_t Length;
    
    String(uint32_t length) {
        Length = length;
        //        Type = CurrentIsolate->StringType;
    }
};

    


struct CompilationUnit {
    uint32_t SizeOfUnit;
    uint16_t SizeOfRegisters;
    uint16_t SizeOfInitializedRegisters;
    
    VALUE* StartOfConstants() {   return (VALUE*)((byte*)this + sizeof(CompilationUnit)); }
    Instruction* StartOfInstructions() { return (Instruction*)((byte*)this + sizeof(CompilationUnit) + SizeOfInitializedRegisters); }
    int GetInitializedRegisterCount() { return SizeOfInitializedRegisters / sizeof(VALUE); }
    
    CompilationUnit( Instruction* code, VALUE* registers, int sizeUninit ) {
        SizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
        SizeOfRegisters = SizeOfInitializedRegisters + sizeUninit;
    }
};
    
    struct Compilation {
        uint32_t SizeOfCompilation;
        
        CompilationUnit* GetFirstCompilationUnit() {
            return (CompilationUnit*)((byte*)this + sizeof(Compilation));
        }
        
        uintptr_t GetLastByteAddress() {
            return (uintptr_t)this + SizeOfCompilation - 1;
        }
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
    CompilationUnit* Comp;
    
    VALUE* GetStartOfRegisters() {
        return (VALUE*)(((byte*)this) + sizeof(Frame));
    }
};
        
extern ATTRIBUTE_TLS Isolate* CurrentIsolate;



// Continuations are used by value as it is only 16 bytes in size (on 64 bits architectures).
class Continuation {
public:
    Instruction* PC;                    // Program Counter (aka Instruction Pointer).
    Frame* frame = NULL;
    
    inline VALUE GetReturnValue() {
        return frame->GetStartOfRegisters()[0];
    }
    
    inline bool HasRunToCompletion() {
        return (*PC).OP == END;
    }
    
    void EnterIntoNewFrame( CompilationUnit* code, Frame* parent ) {
        assert( frame == NULL );
        // The compiled code contains the size of the register machine needed for the
        // code. It also contains the initial values for the registers that are either
        // invariant or that have a initial value.
        frame = (Frame*)CurrentIsolate->AdvanceStack(sizeof(Frame) + code->SizeOfRegisters);
        memcpy( ((byte*)frame) + sizeof(Frame), ((byte*)code) + sizeof(CompilationUnit), code->SizeOfInitializedRegisters );
        frame->Comp = code;
        frame->Parent = parent;
    }
};



class Interpreter {
public:
    
    static Continuation Interpret( Isolate* isolate, Continuation cont );
    
    
    static Continuation Interpret( Isolate* isolate, Compilation* code ) {
        Continuation c;
        CompilationUnit* unit = code->GetFirstCompilationUnit();
        c.EnterIntoNewFrame(unit, NULL);
        c.PC = unit->StartOfInstructions();
        return Interpreter::Interpret( isolate, c );
    }
    
};
    

} } // namespace Addie::Internals





#endif /* Addie_hpp */
