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
// https://github.com/Starcounter-Jack/Addie/wiki

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
    VALUE a1,a2,a3,a4;

    
    while (true) {
        
        
        Instruction i = (*p);
        
        // std::cout << "OPERATION!!! " << (int)i.OP;
        
        switch (i.OP) {
            case END:
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
            case (SCALL_0):
                sym = r[i.B].SymbolId;
                // TODO!
                p++;
                break;
            case (SCALL_1):
                sym = r[i.B].SymbolId;
                a1 = r[i.C];
                // TODO!
                if (sym == SymPrint) {
                    std::cout << "\nPrinting " << a1.Print() << "\n";
                }
                p++;
                break;
            case (SCALL_2):
                //std::cout << "CALL-1";
                sym = r[i.B].SymbolId;
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                // TODO!
                if (sym == SymPrint) {
                    std::cout << "\nPrinting " << a1.Print() << ", " << a2.Print() << "\n";
                }
                
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
            case (SCALL_3):
                
                sym = r[i.B].SymbolId;
                str = isolate->GetStringFromSymbolId(sym);
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                if (sym == SymPlus) {
                    sum = a1.Integer + a2.Integer + a3.Integer;
                    r[i.A] = INTEGER(sum);
                }
                p++;
                break;
            case (SCALL_4):
                
                sym = r[i.B].SymbolId;
                str = isolate->GetStringFromSymbolId(sym);
                a1 = r[i.C];
                p++;
                a2 = r[(*p).OP];
                a3 = r[(*p).A];
                a4 = r[(*p).B];
                if (sym == SymPlus) {
                    sum = a1.Integer + a2.Integer + a3.Integer + a4.Integer;
                    r[i.A] = INTEGER(sum);
                }
                p++;
                break;
            case (DEREF):
                sym = r[i.B].SymbolId;
                std::cout << "Dereferencing r[" << + (unsigned int)i.B << "] (" << isolate->GetStringFromSymbolId(sym) << ")\n";
                if (str.compare("y")) {
                    r[i.A] = INTEGER(5);
                }
                p++;
                break;
            default:
                std::cout << "Unknown: " << isolate->GetStringFromSymbolId(i.OP) << "\n" ;
                p++;
                break;
                //                   throw std::runtime_error("Illegal OP code");
                //                   break;
        }
    }
end:
    //return cont->Checkpoint();
    cont.PC = p;
    return cont;
    
}