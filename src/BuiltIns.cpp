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
    int ret = 0;
    for (int t=0;t<args;t++) {
        VALUE v = c->GetArgument(t);
        assert(v.IsInteger());
        ret += v.Integer;
    }
    return INTEGER(ret);
}


VALUE Print(Continuation* c, int args ) {
    for (int t=0;t<args;t++) {
        VALUE v = c->GetArgument(t);
        std::cout << v.Print();
    }
    return NIL();
}

struct {
    const char* name;
    BuiltInFunction func;
} builtIns [] = {
    { "+", Add },
    { "print", Print }
};
#define BUILTIN_COUNT 2



void Isolate::RegisterBuiltInFunctions() {
    Symbol sym;
    for (int t=0;t<BUILTIN_COUNT;t++) {
        sym = RegisterSymbol(builtIns[t].name);
        RegisterBuiltInFunction( sym, builtIns[t].func );
    }
}

