//
//  Isolate.hpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Isolate_hpp
#define Isolate_hpp

#include "Value.hpp"
#include <map>
#include <vector>
#include <sys/mman.h>
#include <cstring>


enum Symbols {
    END,
//    SET_REGISTER_WINDOW,
    EXIT_WITH_CONTINUATION,
    RETURN,
    LOAD_CONST,
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
    "load-const",
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
    //byte* Code;
    byte* Constants;
    byte* Heap;
    
    uint64_t NextOnHeap;
    //uint64_t NextOnCode;
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
    
    
    void* MallocConstant( size_t size ) {
        void* newAddress = (void*)NextOnConstant;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnConstant += size;
        NextOnConstant = (NextOnConstant + 0b01111) & ~0b01111; // Round up for alignment
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
    
    void* MallocHeap( size_t size ) {
        void* newAddress = (void*)NextOnHeap;
        BytesAllocated += size;
        NumberOfAllocations++;
        NextOnHeap += size;
        NextOnHeap = (NextOnHeap + 0b01111) & ~0b01111; // Round up for alignment
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



#endif /* Isolate_hpp */
