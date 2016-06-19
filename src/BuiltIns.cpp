//
//  BuiltIns.cpp
//  Addie
//
//  Created by Joachim Wester on 19/06/2016.
//
//

#include "VM.hpp"

using namespace Addie;
using namespace Addie::Internals;


VALUE Add(Continuation* c, int args ) {
    std::cout << "\nAdding! ";
    int ret = 0;
    for (int t=0;t<args;t++) {
        VALUE v = c->GetArgument(t);
        assert(v.IsInteger());
        ret += v.Integer;
    }
    return INTEGER(ret);
}

void RegisterBuiltInFunctions( Isolate* isolate ) {
    isolate->RegisterFunction( "+", Add );
}


VALUE Addie::Internals::CallBuiltInFunction( Continuation* c, Symbol func, int args ) {
    //VALUE* r =  c->frame->GetStartOfRegisters(); // cframe->StartOfRegisters();
    //int ret;
    switch (func) {
        case (SymPrint):
            std::cout << "\nPrinting! ";
            for (int t=0;t<args;t++) {
                std::cout << c->GetArgument(t).Print();
            }
            std::cout << "\n";
            return NIL();
        case (SymPlus):
            return Add( c, args );
    }
    throw std::runtime_error("Not implemented");
}