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

// To allow multiple VMs in the same process.
class Isolate {
    
public:
    std::map<std::string,uint16_t> SymbolsIds;  // { firstname:1, lastname:2, foo:3, bar:4 }
    std::vector<std::string> SymbolStrings;     // [ "firstname", "lastname", "foo", "bar" ]
    
    VALUE EmptyList;
    NIL Nil;
    
    uint LastSymbolUsed = 0;
    
    std::string GetStringFromSymbolId( uint16_t id ) {
        return SymbolStrings[id];
    }
    
    uint RegisterSymbol( const char* str, size_t size ) {
//        std::cout << std::string( str, size );
        
        auto s = std::string(str,size);
        
        auto x = SymbolsIds.find(str);
        if (x != SymbolsIds.end())
            return x->second;
        
        SymbolStrings.push_back(s);
        uint16_t id = SymbolStrings.size();
        SymbolsIds[s] = id;
        
        return SymbolStrings.size();
    }
    
    Isolate()
    {
    }
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
