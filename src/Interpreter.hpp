//
//  Interpreter.hpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <stdio.h>
#include "Value.hpp"
#include "Isolate.hpp"
#include "Vector.hpp"
#include "VM.hpp"


class Interpreter {
public:

    static Continuation Interpret( Continuation cont ) {
        
        Instruction* p = cont.PC;
        Instruction* start = p;
        VALUE* r = (VALUE*)(((byte*)cont.frame) + sizeof(Frame));
        
        std::cout << "\nResuming execution at " << p << "\n";
        
        int regCount = cont.frame->Comp->SizeOfRegisters/sizeof(VALUE);
        
        Symbol sym;
        uintptr_t address;
        
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
               case (CALL_0):
                   //std::cout << "CALL-0";
                   //std::cout << " Symbol=";
                   //std::cout << r[i.A].Integer;
                   //std::cout << "\n";
                   p++;
                   break;
               case (CALL_1):
                   //std::cout << "CALL-1";
                   sym = r[i.A].Integer;
                   if (sym == SymPrint) {
                       std::cout << "\nPrinting ";
                       r[0].Print();
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
               case (CALL_2):
//                   std::cout << "CALL-2";

                    sym = r[i.A].Integer;
                   if (sym == SymPlus) {
                      r[0] = INTEGER(r[i.B].Integer + r[i.C].Integer);
                   }
                   
                   p++;

                   break;
               default:
                   std::cout << "Unknown!";
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
    
    
    static Continuation Interpret( Compilation* code ) {
        Continuation c;
        c.EnterIntoNewFrame(code, NULL);
        c.PC = code->Code;
        return Interpreter::Interpret( c );
    }
    
};

#endif /* Interpreter_hpp */
