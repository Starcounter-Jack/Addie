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
#include "Heap.hpp"

// To allow multiple VMs in the same process.
class Isolate {
    
public:
    std::map<std::string,uint16_t> SymbolsIds;  // { firstname:1, lastname:2, foo:3, bar:4 }
    std::vector<std::string> SymbolStrings;     // [ "firstname", "lastname", "foo", "bar" ]
    
    VALUE EmptyList;
    NIL Nil;
    Heap Heap;
    
    uint LastSymbolUsed = 0;
    
    std::string GetStringFromSymbolId( uint16_t id ) {
        return SymbolStrings[id];
    }

    uint RegisterSymbol( const char* str ) {
        return RegisterSymbol( str, strlen(str) );
    }
    
    uint RegisterSymbol( const char* str, size_t size, int known = -1 ) {
//        std::cout << std::string( str, size );
        
        auto s = std::string(str,size);
        
        auto x = SymbolsIds.find(str);
        if (x != SymbolsIds.end())
            return x->second;
        
        SymbolStrings.push_back(s);
        uint16_t id = SymbolStrings.size();
        SymbolsIds[s] = id;
        
        if (known != -1 && known != id ) {
            throw std::runtime_error("Error in fixed symbol registration");
        }
        
        return SymbolStrings.size();
    }

    //rrrr
    //rrra
    //rrar
    //rraa
    //rara
    //arar
    
    Isolate()
    {
#define MOVE_rr 1
        RegisterSymbol( "MOVE_rr", 1 );
#define MOVE_ra 2
        RegisterSymbol( "MOVE_ra", 2 );
#define MOVE_aa 3
        RegisterSymbol( "MOVE_aa", 3 );
#define LOAD 4
        RegisterSymbol( "LOAD", 4 );
#define CALL 5
        RegisterSymbol( "CALL", 5 );
#define GOTO 6
        RegisterSymbol( "GOTO", 6 );
#define RETURN 7
        RegisterSymbol( "RETURN", 7 );
#define IF_TRUE_GOTO 8
        RegisterSymbol( "IF_TRUE_GOTO", 8 );
#define IF_FALSE_GOTO 9
        RegisterSymbol( "IF_FALSE_GOTO", 9 );
#define CLOSURIZE 10
        RegisterSymbol( "CLOSURIZE", 10 );
        
        
#define NilSymbol 11
        RegisterSymbol( "nil", 11 );
#define TrueSymbol 12
        RegisterSymbol( "true", 12 );
#define FalseSymbol 13
        RegisterSymbol( "false", 13 );
        
//        ReservedSymbols = 8;
    }
    
    
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


#endif /* Isolate_hpp */
