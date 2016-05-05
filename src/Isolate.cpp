//
//  Isolate.cpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "VM.hpp"
#include "Reader.hpp"

using namespace Addie::Internals;


Isolate::Isolate() {
    
    
    // TODO! This needs to change. We cannot expect these ranges to be available.
    // Nice for debugging however.
    Stack = ReserveMemoryBlock(0xaaffe000000, 0x10000000000);
    Constants = ReserveMemoryBlock(0xbaffe000000, 0x10000000000);
    Heap = ReserveMemoryBlock(0xcaffe0000,0x10000000000);
    
    NextOnStack = (uintptr_t)Stack;
    NextOnHeap = (uintptr_t)Heap;
    NextOnConstant = (uintptr_t)Constants;
    
    // Register the known symbols. Each VM bytecode OP also has a corresponding
    // text mnemonic such that the first 256 symbols correspond to a OP code.
    for (int t=0;t<Sym_Count;t++) {
        RegisterSymbol( SymStrings[t], strlen(SymStrings[t]), t );
    }
}

void Isolate::PrintStatus() {
    if (NumberOfAllocations) {
        std::cout << "\nNumber of unfreed allocations: ";
        std::cout << NumberOfAllocations;
        std::cout << "\nTotal allocations: ";
        std::cout << BytesAllocated;
        std::cout << " bytes\n";
    }
}

Symbol Isolate::RegisterSymbol( const char* str, size_t size, int known ) {
    auto s = std::string(str,size);
    auto x = SymbolsIds.find(str);
    if (x != SymbolsIds.end()) {
        return x->second;
    }
    
    SymbolStrings.push_back(s);
    uint16_t id = SymbolStrings.size() - 1;
    SymbolsIds[s] = id;
    
    if (known != -1 && known != id ) {
        throw std::runtime_error("Error in fixed symbol registration");
    }
    return id;
}




ATTRIBUTE_TLS Isolate* Addie::Internals::CurrentIsolate;