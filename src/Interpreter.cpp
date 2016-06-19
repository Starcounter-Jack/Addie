//
//  Interpreter.cpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "VM.hpp"

using namespace Addie::Internals;


// VM byte-codes are described here
// https://github.com/Starcounter-Jack/Addie/wiki/Byte-Code


VALUE CallBuiltInFunction0( Isolate* isolate, Continuation* c, Symbol func ) {
    VALUE ret;
    VALUE* r;
    c->EnterIntoNewRuntimeFrame(isolate, c->frame );
    r = c->frame->GetStartOfRegisters();
    ret = CallBuiltInFunction(isolate,c,func,0);
    c->ExitRuntimeFrame(isolate);
    return ret;
}

VALUE CallBuiltInFunction1( Isolate* isolate, Continuation* c, Symbol func, VALUE a1 ) {
    VALUE ret;
    VALUE* r;
    c->EnterIntoNewRuntimeFrame(isolate, c->frame );
    r = c->frame->GetStartOfRegisters();
    r[1] = a1;
    ret = CallBuiltInFunction(isolate,c,func,1);
    c->ExitRuntimeFrame(isolate);
    return ret;
}


VALUE CallBuiltInFunction2( Isolate* isolate, Continuation* c, Symbol func, VALUE a1, VALUE a2 ) {
    VALUE ret;
    VALUE* r;
    c->EnterIntoNewRuntimeFrame(isolate, c->frame );
    r = c->frame->GetStartOfRegisters();
    r[1] = a1;
    r[2] = a2;
    ret = CallBuiltInFunction(isolate,c,func,2);
    c->ExitRuntimeFrame(isolate);
    return ret;
}



VALUE CallBuiltInFunction3( Isolate* isolate, Continuation* c, Symbol func, VALUE a1, VALUE a2, VALUE a3 ) {
    VALUE ret;
    VALUE* r;
    c->EnterIntoNewRuntimeFrame(isolate, c->frame );
    r = c->frame->GetStartOfRegisters();
    r[1] = a1;
    r[2] = a2;
    r[3] = a3;
    ret = CallBuiltInFunction(isolate,c,func,3);
    c->ExitRuntimeFrame(isolate);
    return ret;
}


VALUE CallBuiltInFunction4( Isolate* isolate, Continuation* c, Symbol func, VALUE a1, VALUE a2, VALUE a3, VALUE a4 ) {
    VALUE ret;
    VALUE* r;
    c->EnterIntoNewRuntimeFrame(isolate, c->frame );
    r = c->frame->GetStartOfRegisters();
    r[1] = a1;
    r[2] = a2;
    r[3] = a3;
    r[4] = a4;
    ret = CallBuiltInFunction(isolate,c,func,4);
    c->ExitRuntimeFrame(isolate);
    return ret;
}


Continuation Interpreter::Interpret( Isolate* isolate, Continuation cont ) {
    
    Instruction* p = cont.PC;
    //        Instruction* start = p;
    VALUE* r = cont.frame->GetStartOfRegisters();
    
    std::cout << "Resuming execution at " << p << "\n";
    
    //        int regCount = cont.frame->Comp->SizeOfRegisters/sizeof(VALUE);
    
    Symbol sym;
    int sum;
    std::string str;

    //        uintptr_t address;
    VALUE a1,a2,a3,a4,a5;
    VALUE fn;
    
    while (true) {
        
        
        Instruction i = (*p);
        
        // std::cout << "OPERATION!!! " << (int)i.OP;
        
        switch (i.OP) {
            case RET:
                goto end;
                /*
                 case SET_REGISTER_WINDOW:
                 p++;
                 std::cout << "set-reg-win";
                 std::cout << "      (";
                 std::cout << (*((VALUE*)p)).Integer;
                 std::cout << ")";
                 address = (uintptr_t)(*((VALUE*)p)).Integer;
                 r = (VALUE*)address;
                 p = (Instruction*)(((VALUE*)p) + 1); // Move IP past the address
                 
                 for (int t=0;t<regCount;t++) {
                 std::cout << "R[";
                 std::cout << t;
                 std::cout << "]=";
                 std::cout << r[t].Integer;
                 std::cout << "\n";
                 }
                 
                 break;
                 */
                
            case EXIT_WITH_CONTINUATION:
                p++;
                //std::cout << "\nexit-with-continuation @(" << p << ")";
                goto end;
       //     case (CALL_SYSTEM):
       //         r[i.C] = CallBuiltInFunction(r[i.B].SymbolId,i.C,r);
       //         p++;
       //         break;

                
            case (SCALL_0):
                sym = r[i.B].SymbolId;
                r[i.A] = CallBuiltInFunction0( isolate, &cont, sym );

                // TODO!
                p++;
                break;

            case (JMP):
                p += i.A3;
                break;
                
            case (JMP_IF_TRUE):
                if (r[0].Integer) {
                    p += i.A3;
                }
                break;
                
            case (MOVE):
                std::cout << "MOVE";
                std::cout << "        (";
                std::cout << (int)p->A;
                std::cout << ",";
                std::cout << (int)p->B;
                std::cout << ")";
                p++;
                
                break;
                
            case (SCALL_1):
                sym = r[i.B].SymbolId;
                a1 = r[i.C];
                r[i.A] = CallBuiltInFunction1( isolate, &cont, sym, a1 );
                
            //    r[i.A] = CallBuiltInFunction1(sym, 1, r);
                
                p++;
                break;
                
            case (SCALL_2):
                
                sym = r[i.B].SymbolId;
                //str = isolate->GetStringFromSymbolId(sym);
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                r[i.A] = CallBuiltInFunction2( isolate, &cont, sym, a1, a2 );

                p++;
                break;
                
            case (SCALL_3):
                
                sym = r[i.B].SymbolId;
                //str = isolate->GetStringFromSymbolId(sym);
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                
                r[i.A] = CallBuiltInFunction3( isolate, &cont, sym, a1, a2, a3 );
//                throw std::runtime_error("Not implemented");
                
                p++;
                break;
                
            case (SCALL_4):
                
                sym = r[i.B].SymbolId;
                //str = isolate->GetStringFromSymbolId(sym);
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                a4 = r[(*p).B];
                
                r[i.A] = CallBuiltInFunction4( isolate, &cont, sym, a1, a2, a3, a4 );
//                throw std::runtime_error("Not implemented");

                p++;
                break;
                
            case (DEREF):
                sym = r[i.B].SymbolId;
                std::cout << "Dereferencing r[" << + (unsigned int)i.B << "] (" << isolate->GetStringFromSymbolId(sym) << ")\n";
                throw std::runtime_error("Not implemented");

                p++;
                break;
                
            case (CALL_0):
                fn = r[i.B];
                
                cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                p = cont.PC;
                r = cont.frame->GetStartOfRegisters();

                
                break;
                
            case (CALL_1):
                fn = r[i.B];
                a1 = r[i.C];
                
                cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                p = cont.PC;
                r = cont.frame->GetStartOfRegisters();
                
                r[1] = a1;
                break;
                
            case (CALL_2):
                fn = r[i.B];
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];

                cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                p = cont.PC;
                r = cont.frame->GetStartOfRegisters();

                r[1] = a1;
                r[2] = a2;
                break;
                
            case (CALL_3):
                fn = r[i.B];
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                
                if (fn.Integer == 123) {
                    sum = a1.Integer + a2.Integer + a3.Integer;
                    r[i.A] = INTEGER(sum);
                }
                else {
                    cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                    p = cont.PC;
                    r = cont.frame->GetStartOfRegisters();
                    
                    r[1] = a1;
                    r[2] = a2;
                    r[3] = a3;
                }
                break;
                
            case (CALL_4):
                fn = r[i.B];
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                a4 = r[(*p).B];
                

                cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                p = cont.PC;
                r = cont.frame->GetStartOfRegisters();
                
                r[1] = a1;
                r[2] = a2;
                r[3] = a3;
                r[4] = a4;
                
                break;

                
            case (CALL_5):
                fn = r[i.B];
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                a4 = r[(*p).B];
                a5 = r[(*p).C];
                
                
                cont.EnterIntoNewRuntimeFrame(isolate, (CodeFrame*)fn.OtherPointer, cont.frame );
                p = cont.PC;
                r = cont.frame->GetStartOfRegisters();
                
                r[1] = a1;
                r[2] = a2;
                r[3] = a3;
                r[4] = a4;
                r[5] = a5;
                
                break;
                
            case (DEF):
                sym = r[i.B].SymbolId;
                std::cout << "Defining " << isolate->GetStringFromSymbolId(sym) << "\n";
                r[i.A] = r[i.C];
                p++;
                break;
                
            default:
                std::cout << "Unknown: " << isolate->GetStringFromSymbolId(i.OP) << "\n" ;
                p++;
               // break;
                                   throw std::runtime_error("Illegal OP code");
                                   break;
        }
    }
end:
    //return cont->Checkpoint();
    cont.PC = p;
    return cont;
    
}


VALUE Addie::Internals::CallBuiltInFunction( Isolate* isolate, Continuation* c, Symbol sym, int args ) {
    //VALUE* r =  c->frame->GetStartOfRegisters(); // cframe->StartOfRegisters();
    //int ret;
    BuiltInFunction func = isolate->GetBuiltInFunction( sym );
    if (func == NULL ) {
        std::cout << "Cannot find function ";
        std::cout << isolate->GetStringFromSymbolId(sym);
        std::cout << "\n";
        assert( func != NULL);
    }
    return func( c, args );
}