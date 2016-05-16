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


byte* CompileForm( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE form );

// Find all declared variables and their default values
byte* CompileLet( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE form ) {

    
//    CompilationUnit* unit = (CompilationUnit*)bytecode;
//    bytecode += sizeof(CompilationUnit);

//    form = form.Rest();
//    bytecode = CompileLetFrame(isolate, bytecode, mf, form.First() );
    VALUE lets = form.Rest().First();

    
//    auto newMf = MALLOC_HEAP(Metaframe); // TODO! GC
//    new (newMf) Metaframe( mf );

//    if (mf != NULL ) {
//        // We nest all the frames for lexical scoping. Any variable not bound locally, will
//        // be added as an implicit argument. These implicit arguments allows closures
//        // to refer to variables in their parent scope.
//        newMf->Parent = mf;
//    }

    VALUE* registers = mf->compilationUnit->StartOfRegisters();
    assert( registers == (VALUE*)bytecode );
    (*registers++) = NIL();
    int cnt = lets.Count();
    int varCount = 0;
    for (int t=0;t<cnt;t += 2) {
        varCount++;
        Symbol variableName = lets.GetAt(t).SymbolId;
        //std::cout << "Found r[" << varCount << "] as local variable " << lets.GetAt(t).Print() << "=" << lets.GetAt(t+1).Print() << "\n";
        //mf->LocalsAndArguments.push_back(Variable(variableName,form.GetAt(t+1)));
        mf->Registers.push_back(variableName);
        uint8_t regno = mf->Registers.size()-1;
        mf->compilationUnit->ReportLocals(1);
        mf->Bindings[variableName] = Binding(mf,regno);
        (*registers++) = lets.GetAt(t+1);
    }
    bytecode = (byte*)registers;
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        std::cout << "Statements following let:" << form.First().Print() << "\n";
        bytecode = CompileForm(isolate,bytecode,mf,form.First());
        form = form.Rest();
    }
    
//    new (unit) CompilationUnit( code, registers,  );

    return bytecode;
}

/*

Metaframe* CompileFrames( Isolate* isolate, Namespace* ns, Metaframe* mf, VALUE form ) {
    
    
    if (form.Type == TList && form.ListStyle == QParenthesis) {
        VALUE fst = form.First();
        if (fst.IsSymbol()) {
            Symbol verb = fst.SymbolId;
            switch (verb) {
                case SymLetStar:
                    return CompileLetFrame( isolate, ns, mf, form.Rest().First() );
                    break;
            }
        }
        mf->Body = form;
    }
    return mf;
}

*/

byte* CompilePrototype( Isolate* isolate, Metaframe* mf, byte* p, VALUE form ) {
    
//    byte* start = p; //(byte*)isolate->NextOnConstant;
    //byte* p = (byte*)start;
    
    Instruction* code;
    Instruction* c;
    VALUE* registers;
    VALUE* r;
    int uninitatedRegisters;
    
    //    CompilationUnit* header = (CompilationUnit*)p;
//    p += sizeof(CompilationUnit);
    CompilationUnit* header = mf->compilationUnit;
    
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
    p = (byte*)(c-1);
    
    header->SizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
    header->SizeOfRegisters = header->SizeOfInitializedRegisters + uninitatedRegisters * sizeof(VALUE);
    
    //isolate->NextOnConstant = (u64)p; // Mark the memory as used
    
    return p;
    
    //return header;
}



byte* CompileConstant( Isolate* isolate, Metaframe* mf, byte* p, VALUE form ) {
    
    Instruction* code, *c;
    VALUE* registers, *r;
    
    CompilationUnit* unit = mf->compilationUnit; // (CompilationUnit*)p;
//    p += sizeof(CompilationUnit);
    
    //registers = r = (VALUE*)p;
    
    // Write register initialization
    //*((VALUE*)r++) = form; // R0 is the return value
    
    // Write code instructions
    code = c = (Instruction*)p; //unit->Get
    *(c++) = Instruction(END);
    p = (byte*)(c-1);
    
    // Write header
//    new (unit) CompilationUnit( code, registers, 0 );
    unit->SetReturnRegister( form );
    
    return p;
}

byte* CompileSymbol( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE symbol ) {
    Instruction* i = (Instruction*)bytecode;
    (*i++) = Instruction( SymDedef, 0 );
    //    throw std::runtime_error("Not Implemented");
    return (byte*)i;
}

byte* CompileParenthesis( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE form ) {
    if (form.IsEmpty()) {
        // () evaluates to an empty list whereas other lists are treated as/
        // function/procedure calls.
        return CompileConstant(isolate, mf, bytecode, form );
    }
    VALUE function = form.First();
    if (function.IsSymbol()) {
        // This is a regular predefined function/procedure call such as (print 123)
        switch (function.SymbolId) {
            case (SymLetStar):
                return CompileLet( isolate, bytecode, mf, form );
            default:
                std::cout << "Function call: " << form.Print() << "\n";
                bytecode = CompileForm(isolate,bytecode,mf,function);
                // mf->FindBinding( function.Symbol );
                Instruction* i = (Instruction*)bytecode;
                (*i++) = OpCall(0);
                return (byte*)i;
        }
    }
//    return CompileConstant(isolate, bytecode, form );
    // This is a direct lambda call such as ((fn [x] (print x)) 123)
    throw std::runtime_error( "Not implemented");
}

byte* CompileList( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE form ) {
    // TODO! We don't know that this list is a constant. Temporary code.
    return CompileConstant(isolate, mf, bytecode, form);
}

byte* CompileForm( Isolate* isolate, byte* bytecode, Metaframe* mf, VALUE form ) {
    switch (form.Type) {
        case TNumber:
            return CompileConstant( isolate, mf, bytecode, form );
        case TList:
            switch (form.ListStyle) {
                case QParenthesis:
                    return CompileParenthesis( isolate, bytecode, mf, form );
                case QCurly:
                case QBracket:
                    return CompileList( isolate, bytecode, mf, form );
                case QString:
                    return CompileConstant( isolate, mf, bytecode, form );
            }
        case TAtom:
            switch (form.AtomSubType) {
                case ANil:
                case AKeyword:
                    return CompileConstant( isolate, mf, bytecode, form );
                case ASymbol:
                    return CompileSymbol(isolate, bytecode, mf, form);
                case AOther:
                    break;
            }
        default:
            break;
    }
    throw std::runtime_error("Compilation error");
}

Compilation* Compiler::Compile( Isolate* isolate, VALUE form ) {
    
    Metaframe* mf = MALLOC_HEAP(Metaframe);
    new (mf) Metaframe(NULL);
    
    byte* p = (byte*)isolate->NextOnConstant;
    auto comp = new (p) Compilation();
    p += sizeof(Compilation);
    
    mf->compilationUnit = (CompilationUnit*)p;
    mf->compilation = comp;
    new (mf->compilationUnit) CompilationUnit();  //( Instruction* code, VALUE* registers, int sizeUninit )
    
    p += sizeof(CompilationUnit);
    
    p = CompileForm( isolate, p, mf, form );
    //return Compile2( isolate, form );
    
    comp->SizeOfCompilation = p - (byte*)comp + 1;
    
    isolate->ReportConstantWrite( (uintptr_t)p ); // Mark the memory as used
    return comp;
}


uintptr_t DisassembleUnit( Isolate* isolate, std::ostringstream& res, CompilationUnit* code ) {
    
    
    
    const char* str;
    VALUE* R = code->StartOfRegisters();
    
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
    return (uintptr_t)p;
}

STRINGOLD Compiler::Disassemble( Isolate* isolate, Compilation* compilation ) {
    
    CompilationUnit* code = compilation->GetFirstCompilationUnit();
    std::ostringstream res;

    uintptr_t p = (uintptr_t)code;
    uintptr_t end = compilation->GetLastByteAddress();
    while (p < end ) {
       p = DisassembleUnit( isolate, res, (CompilationUnit*)p );
    }
    
    return STRINGOLD(res.str());

}
