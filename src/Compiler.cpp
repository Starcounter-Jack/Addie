//
//  Compiler.cpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "VM.hpp"
#include "Compiler.hpp"
#include <sstream>

using namespace Addie;
using namespace Addie::Internals;

// Find all declared variables and their default values
Metaframe* AnalyseLet( Isolate* isolate, Namespace* ns, Metaframe* mf, VALUE form ) {
    int cnt = form.Count();
    int varCount = 0;
    for (int t=0;t<cnt;t += 2) {
        varCount++;
        Symbol variableName = form.GetAt(t).SymbolId;
        std::cout << "Found r[" << varCount << "] as local variable " << form.GetAt(t).Print() << "=" << form.GetAt(t+1).Print() << "\n";
        //mf->LocalsAndArguments.push_back(Variable(variableName,form.GetAt(t+1)));
        mf->LocalsAndArguments.push_back(variableName);
        mf->Bindings[variableName] = mf->LocalsAndArguments.size()-1;
    }
    return mf;
}

Metaframe* Compiler::Analyse( Isolate* isolate, Namespace* ns, VALUE form ) {
    
    auto mf = MALLOC_HEAP(Metaframe); // TODO! GC
    new (mf) Metaframe();
    
    if (form.Type == TList && form.ListStyle == QParenthesis) {
        VALUE fst = form.First();
        if (fst.IsSymbol()) {
            Symbol verb = fst.SymbolId;
            switch (verb) {
                case SymLetStar:
                    return AnalyseLet( isolate, ns, mf, form.Rest().First() );
                    break;
            }
        }
        mf->Body = form;
    }
    return mf;
}



Compilation* Compiler::Compile( Isolate* isolate, Namespace* ns, VALUE form ) {
    //        int type = form.Type;
    
    auto mf = Analyse(isolate, ns, form );

    
    byte* p = (byte*)isolate->NextOnConstant;
    
    Instruction* code;
    Instruction* c;
    VALUE* registers;
    VALUE* r;
    int uninitatedRegisters;
    
    Compilation* header = (Compilation*)p;
    p += sizeof(Compilation);
    
    registers = r = (VALUE*)p;
    
    if (form.Type == TList && form.ListStyle == QParenthesis) {
        // Function calling form (...)
        VALUE fst = form.First();
        if (fst.IsSymbol()) {
            
            r[0] = NIL();             // R0 retval
            r[1] = form.First();    // R1
            
            uninitatedRegisters = 0;
            r += mf->Bindings.size();
            code = c = (Instruction*)r;
            *(c++) = OpCall(1);
            *(c++) = Instruction(END);
            p = (byte*)c;
            goto end;

//            return CompilePrototype( isolate, form );
        } else {
            throw std::runtime_error("Cannot compile");
        }
    }
    else {
    
        *((VALUE*)r++) = form;             // R0 retval
        uninitatedRegisters = 0;
    
        code = c = (Instruction*)r;
        *(c++) = Instruction(END);       // 3=Print 5=internadiate1
        p = (byte*)c;
    }
    
end:
    header->SizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
    header->SizeOfRegisters = header->SizeOfInitializedRegisters + uninitatedRegisters * sizeof(VALUE);
    
    isolate->ReportConstantWrite( (uintptr_t)p ); // Mark the memory as used
    
    return header;

    
}

Compilation* Compiler::CompilePrototype( Isolate* isolate, VALUE form ) {
    
    byte* start = (byte*)isolate->NextOnConstant;
    byte* p = (byte*)start;
    
    Instruction* code;
    Instruction* c;
    VALUE* registers;
    VALUE* r;
    int uninitatedRegisters;
    
    Compilation* header = (Compilation*)p;
    p += sizeof(Compilation);
    
    registers = r = (VALUE*)p;
    
    std::cout << "\nCompiling (print (+ 10 20))\n";
    
        *((VALUE*)r++) = NIL();             // R0 retval
        *((VALUE*)r++) = SYMBOL(SymPlus);   // R1
        *((VALUE*)r++) = SYMBOL(SymPrint);  // R2
        *((VALUE*)r++) = INTEGER(10);       // R3
        *((VALUE*)r++) = INTEGER(20);       // R4
    
        // R5
        
        code = c = (Instruction*)r;
        
        //            *(c++) = Instruction(SET_REGISTER_WINDOW);     // CODE START 0=SymPlus 1=10 2=20
        //            uint64_t temp1 = (uint64_t)registers;
        
        //            *((VALUE*)c) = INTEGER(temp1);
        //            uint64_t temp2 = (uint64_t)(*((VALUE*)c)).Integer;
        
        //            if (temp1 != temp2 ) {
        //                std::cout << temp1 << "!=" << temp2 << "\n";
        //            }
        
        
        //            std::cout << (*((VALUE*)c)).Integer;
        //            c += sizeof(INTEGER)/sizeof(Instruction);
        *(c++) = OpCall(1,3,4);     // CODE START 0=SymPlus 1=10 2=20
        //*(c++) = OpMove(0,5);       // retval -> intermediate1
        *(c++) = Instruction(EXIT_WITH_CONTINUATION);
        *(c++) = OpCall(2,0);       // 3=Print 5=internadiate1
        *(c++) = Instruction(END);       // 3=Print 5=internadiate1

        //*(c++) = Instruction(JMP_IF_TRUE,(int32_t)-1);       // 3=Print 5=internadiate1
        uninitatedRegisters = 1;
        p = (byte*)c;
    
    header->SizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
    header->SizeOfRegisters = header->SizeOfInitializedRegisters + uninitatedRegisters * sizeof(VALUE);
    
    isolate->NextOnConstant = (u64)p; // Mark the memory as used
    
    return header;
}

STRINGOLD Compiler::Disassemble( Isolate* isolate, Compilation* code ) {
    
    //Compilation* header = (Compilation*)code;
    std::ostringstream res;
    const char* str;
    VALUE* R = code->StartOfConstants();
    
    int prefix = 6;
    res << "============================================================\n";
    res << "START:";
    
    
    Instruction* p = code->StartOfInstructions();
    
    while (true) {
        
        while (prefix++ < 12) res << " ";
        
        res << ".";
        str = isolate->GetStringFromSymbolId((*p).OP).c_str();
        
        switch (p->OP) {
            case (END):
                res << str;
                goto end;
                /*
                 case (SET_REGISTER_WINDOW):
                 p++;
                 res << str;
                 res << "      (";
                 temp = (VALUE*)p;
                 res << (*temp).Integer;
                 res << ")";
                 p = (Instruction*)(((VALUE*)p) + 1); // Move IP past the address
                 break;
                 */
            case (CALL_0):
                res << str;
                res << "      (r";
                res << (int)p->A;
                res << ")";
                break;
            case (CALL_1):
                res << str;
                res << "      (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (MOVE):
                res << str;
                res << "        (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (CALL_2):
                res << str;
                res << "      (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
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
    int regCount = code->GetInitializedRegisterCount();
    res << "\n------------------------------------------------------------\n";
    for (int t=0;t<regCount;t++) {
        res << "r";
        res << t;
        res << ":         ";
        res << R[t].Print();
        res << "\n";
    }
    res << "============================================================\n";
    
    
    return STRINGOLD(res.str());
}
