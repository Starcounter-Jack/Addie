//
//  Isolate.cpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Isolate.hpp"
#include "Compiler.hpp"
#include "Reader.hpp"
#include "Interpreter.hpp"


Isolate::Isolate() {
    
    
    // TODO! This needs to change. We cannot expect these ranges to be available.
    // Nice for debugging however.
    Stack = ReserveMemoryBlock(0xaaffe000000, 0x10000000000);
    Constants = ReserveMemoryBlock(0xbaffe000000, 0x10000000000);
    Heap = ReserveMemoryBlock(0xcaffe000000,0x10000000000);
    
    NextOnStack = (uintptr_t)Stack;
    NextOnHeap = (uintptr_t)Heap;
    NextOnConstant = (uintptr_t)Constants;
    
    StringType = new Type( SymString );
    ConsType = new Type( SymCons );

    // Register the known symbols. Each VM bytecode OP also has a corresponding
    // text mnemonic such that the first 256 symbols correspond to a OP code.
    for (int t=0;t<Sym_Count;t++) {
        RegisterSymbol( SymStrings[t], strlen(SymStrings[t]), t );
    }
}

/*
Type* Object::GetType() {
        if (Type == SymString) {
            return CurrentIsolate->StringType;
        }
        else if (Type == SymCons) {
            return CurrentIsolate->ConsType;
        }
}
 */

//STRING Isolate::Compile( VALUE form ) {
//    Compiler c;
//    return c.Compile( form );
//}

//STRING Isolate::Decompile( STRING code ) {
//    Compiler c;
//    return c.Decompile( code );
//}

//VALUE Isolate::Read( STRING source ) {
//    const char* str = source.c_str();
//    auto teststr = StringReader(str,strlen(str));
//    return Parser::ParseForm( &teststr );
//}


ATTRIBUTE_TLS Isolate* CurrentIsolate;