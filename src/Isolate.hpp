//
//  Isolate.hpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Isolate_hpp
#define Isolate_hpp

#include "Addie.hpp"
#include <map>
#include <vector>
#include <sys/mman.h>
#include <cstring>


#define MALLOC_HEAP(type) (type*)CurrentIsolate->MallocHeap(sizeof(type));


class Evaluateable : public NamedEntity {
public:
    virtual VALUE Evaluate() = 0;
};

class Variable : public Evaluateable {
};

class Namespace : public NamedEntity {
public:
    std::map<Symbol,Variable*> Variables;
};


class Function : public NamedEntity {
};

class Continuation;


// To allow multiple VMs in the same process.
class Isolate {
    
public:
    byte* Stack;
    byte* Constants;
    byte* Heap;
    
    uint64_t NextOnStack;
    uint64_t NextOnHeap;
    uint64_t NextOnConstant;
    
    
    int NumberOfAllocations = 0;
    int BytesAllocated = 0;

    
    byte* ReserveMemoryBlock(uint64_t address, size_t size) {
        void* addr = (void*)address;
        
        addr = mmap((void *) addr, size,
                    PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        //printf("%p\n", addr);
        if(addr == MAP_FAILED)
            throw std::runtime_error("Addie is trying to reserving 3TB of fixed memory addresses and fails. Jack was being a little bit too optimisitic.");
        return (byte*)addr;
    }
    
    
    void CheckAddress(void* addr) {
        uint64_t i = (uint64_t)addr;
        if ( i & 0b0000000000000000000000000000000000000000000000000000000000001111 ) {
            throw std::runtime_error( "Addresses must be aligned" );
        }
    }
    
    inline void AlignNextHeap() {
        NextOnHeap = (NextOnHeap + 0b01111) & ~0b01111; // Round up for alignment
    }
    
    inline void AlignNextConstant() {
        NextOnConstant = (NextOnConstant + 0b01111) & ~0b01111; // Round up for alignment
    }
    
    
    void* MallocConstant( size_t size ) {
        void* newAddress = (void*)NextOnConstant;
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
    
    
    void* AdvanceStack( size_t size ) {
        void* newAddress = (void*)NextOnStack;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnStack += size;
        NextOnStack = (NextOnStack + 0b01111) & ~0b01111; // Round up for alignment
        CheckAddress(newAddress);
        return newAddress;
    }
    
    inline void ReportHeapWrite( size_t size ) {
        NextOnHeap += size;
        AlignNextHeap();
    }
    
    
    void* MallocHeap( size_t size ) {
        void* newAddress = (void*)NextOnHeap;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnHeap += size;
//        NextOnHeap = (NextOnHeap + 0b01111) & ~0b01111; // Round up for alignment
        AlignNextHeap();
        CheckAddress(newAddress);
        return newAddress;
    }
    
    void PrintStatus() {
        if (NumberOfAllocations) {
            std::cout << "\nNumber of unfreed allocations: ";
            std::cout << NumberOfAllocations;
            std::cout << "\nTotal allocations: ";
            std::cout << BytesAllocated;
            std::cout << " bytes\n";
        }
    }

    
    
    Type* StringType;
    Type* ConsType;
    
    std::map<std::string,Symbol> SymbolsIds;  // { firstname:1, lastname:2, foo:3, bar:4 }
    std::vector<std::string> SymbolStrings;     // [ "firstname", "lastname", "foo", "bar" ]
    std::map<Symbol,Namespace*> Namespaces;
    
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


    Symbol RegisterSymbol( const char* str, size_t size, int known ) {
//        std::cout << std::string( str, size );
        
        auto s = std::string(str,size);
        
        auto x = SymbolsIds.find(str);
        if (x != SymbolsIds.end()) {
//            if (memcmp(str,GetStringFromSymbolId(x->second).c_str(),size) != 0) {
//                std::cout << str;
//                std::cout << "!=";
//                std::cout << GetStringFromSymbolId(x->second).c_str();
//                throw std::runtime_error("Symbol finder does not work");
//            }
            return x->second;
        }
        
        SymbolStrings.push_back(s);
        uint16_t id = SymbolStrings.size() - 1;
        SymbolsIds[s] = id;
        
        //std::cout << "\nRegistering: ";
        //std::cout << SymbolStrings[id];
        
        if (known != -1 && known != id ) {
            throw std::runtime_error("Error in fixed symbol registration");
        }
        
        return id;
    }

    //rrrr
    //rrra
    //rrar
    //rraa
    //rara
    //arar
    
    
    
//    bool IsReservedSymbol( uint16_t id ) {
//        return id <= ReservedSymbols;
//    }
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

extern ATTRIBUTE_TLS Isolate* CurrentIsolate;


class String : public Object {
public:
    uint32_t Length;
    
    String(uint32_t length) {
        Length = length;
//        Type = CurrentIsolate->StringType;
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

#endif /* Isolate_hpp */
