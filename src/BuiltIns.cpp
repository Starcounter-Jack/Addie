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

VALUE Subtract(Continuation* c, int args ) {
    int ret = 0;
    for (int t=0;t<args;t++) {
        VALUE v = c->GetArgument(t);
        assert(v.IsInteger());
        ret -= v.Integer;
    }
    return INTEGER(ret);
}


VALUE Multiply(Continuation* c, int args ) {
    int ret = 1;
    for (int t=0;t<args;t++) {
        VALUE v = c->GetArgument(t);
        assert(v.IsInteger());
        ret *= v.Integer;
    }
    return INTEGER(ret);
}


VALUE Divide(Continuation* c, int args ) {
    VALUE v = c->GetArgument(0);
    assert(v.IsInteger());
    int ret = v.Integer;
    for (int t=1;t<args;t++) {
        v = c->GetArgument(t);
        assert(v.IsInteger());
        ret /= v.Integer;
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

struct builtIn {
    const char* name;
    BuiltInFunction func;
} builtIns [] = {
    { "+", Add },
    { "-", Subtract },
    { "*", Multiply },
    { "/", Divide },
    { "print", Print }
};


void Isolate::RegisterBuiltInFunctions() {
    Symbol sym;
    for (int t=0;t<sizeof(builtIns)/sizeof(builtIn);t++) {
        sym = RegisterSymbol(builtIns[t].name);
        RegisterBuiltInFunction( sym, builtIns[t].func );
    }
}

