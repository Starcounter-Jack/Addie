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
        VALUE* r = (VALUE*)(((byte*)cont.frame) + sizeof(Frame));
        
        std::cout << "\nResuming execution at " << p << "\n";
        
        int regCount = cont.Comp->SizeOfRegisters/sizeof(VALUE);
        for (int t=0;t<regCount;t++) {
            std::cout << "R[";
            std::cout << t;
            std::cout << "]=";
            std::cout << r[t].Integer;
            std::cout << "\n";
        }
        
        Symbol sym;
        
        while (true) {
            

            Instruction i = (*p);

           // std::cout << "OPERATION!!! " << (int)i.OP;

            switch (i.OP) {
               case END:
//                   throw std::runtime_error("Illegal OP code");
                   goto end;
               case EXIT_WITH_CONTINUATION:
                   std::cout << "\nexit-with-continuation @(" << p;
                   p++;
                   cont.PC = p;
                   std::cout << "==" << cont.PC << ")\n";
                   return cont;
                   break;
               case (CALL_0):
                   std::cout << "CALL-0";
                   std::cout << " Symbol=";
                   std::cout << r[i.A].Integer;
                   std::cout << "\n";
                   p++;
                   break;
               case (CALL_1):
                   std::cout << "CALL-1";
                   sym = r[i.A].Integer;
                   if (sym == SymPrint) {
                       std::cout << "\nPrinting ";
                       r[0].Print();
                   }

                   p++;

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
                   std::cout << "CALL-2";

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
        c.AllocateFrame(code);
        c.PC = code->Code;
        return Interpreter::Interpret( c );
    }
    
};

#endif /* Interpreter_hpp */
