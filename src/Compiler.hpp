//
//  Compiler.hpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Compiler_hpp
#define Compiler_hpp

#include <stdio.h>
#include <vector>
#include <sstream>
#include "Value.hpp"
#include "Isolate.hpp"
#include "Compiler.hpp"
#include "VM.hpp"


extern int JitX86( int num );

class Context {
public:
    Context* Parent;

};




// Always use this one as a value as it is only 16 bytes in size.
// At the end co
class Closure {
public:
    Instruction* Code;
    VALUE* Data;
};




class Compiler {
    
    
public:
    
    
    static Compilation* Compile( VALUE form ) {

        byte* start = (byte*)CurrentIsolate->NextOnConstant;
        byte* p = (byte*)start;
        
        Instruction* code;
        Instruction* c;
        VALUE* registers;
        VALUE* r;
        int uninitatedRegisters;

        Compilation* header = (Compilation*)p;
        p += sizeof(Compilation);
        
        registers = r = (VALUE*)p;
        
        if (form.IsCons()) {
            *((VALUE*)r++) = NIL();             // R0 retval
            *((VALUE*)r++) = SYMBOL(SymPlus);   // R1
            *((VALUE*)r++) = SYMBOL(SymPrint);  // R2
            *((VALUE*)r++) = INTEGER(10);       // R3
            *((VALUE*)r++) = INTEGER(20);       // R4
            // R5
            
            code = c = (Instruction*)r;
            *(c++) = OpCall(1,3,4);     // CODE START 0=SymPlus 1=10 2=20
            *(c++) = OpMove(0,5);       // retval -> intermediate1
            *(c++) = Instruction(EXIT_WITH_CONTINUATION);
            *(c++) = OpCall(2,5);       // 3=Print 5=internadiate1
            uninitatedRegisters = 1;
        }
        else {
            throw std::runtime_error("Can only compile list forms right now");
        }
        
        header->SizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
        header->Code = code;
        header->SizeOfRegisters = header->SizeOfInitializedRegisters + uninitatedRegisters * sizeof(VALUE);
        
        CurrentIsolate->NextOnConstant = (u64)p; // Mark the memory as used

        return header;
    }
    
    static STRING Disassemble( Compilation* code ) {
        
        //Compilation* header = (Compilation*)code;
        
        std::ostringstream res;
        int symbol;
        const char* str;

        int prefix = 6;
        res << "============================================================\n";
        res << "START:";
        Instruction* p = code->Code;
        
        while (true) {
            
            while (prefix++ < 12) res << " ";
            
            res << ".";
            str = CurrentIsolate->GetStringFromSymbolId((*p).OP).c_str();
            int i;
            
            switch (p->OP) {
                case (0):
                    res << str;
                    goto end;
                case (CALL_0):
                    res << str;
                    res << "      (";
                    res << (int)p->A;
                    res << ")";
                    break;
                case (CALL_1):
                    res << str;
                    res << "      (";
                    res << (int)p->A;
                    res << ",";
                    res << (int)p->B;
                    res << ")";
                    break;
                case (MOVE):
                    res << str;
                    res << "        (";
                    res << (int)p->A;
                    res << ",";
                    res << (int)p->B;
                    res << ")";
                    break;
                case (CALL_2):
                    res << str;
                    res << "      (";
                    res << (int)p->A;
                    res << ",";
                    res << (int)p->B;
                    res << ",";
                    res << (int)p->C;
                    res << ")";
                    break;
                default:
                    res << str;
                    break;
            }
            res << "\n";

            p++;
            
            prefix = 0;
        }
    end:
        res << "\n============================================================\n";

        return STRING(res.str());
    }
};

#endif /* Compiler_hpp */
