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


int CompileForm( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd );



// Compile a function/lambda declaration
int CompileFn( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    
    assert( mtd == UseReturnRegister);
    
    VALUE args = form.Rest().First();
    
    int cnt = args.Count();
    
    for (int t=0;t<cnt;t++) {
        Symbol argName = args.GetAt(t).SymbolId;
        mf->Registers.push_back(argName);
        //int regno = mf->AddConstant(NIL());
        mf->Bindings[argName] = Binding(mf,mf->AddConstant(NIL()));
    }
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        std::cout << "Statements following fn:" << form.First().Print() << "\n";
        CompileForm( isolate,mf,form.First(), UseReturnRegister );
        form = form.Rest();
    }
    
    //    new (unit) CompilationUnit( code, registers,  );
    
    return 0;
}


// Find all declared variables and their default values
int CompileLet( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {

    assert( mtd == UseReturnRegister );
    
    VALUE lets = form.Rest().First();

    
//    auto newMf = MALLOC_HEAP(Metaframe); // TODO! GC
//    new (newMf) Metaframe( mf );

//    if (mf != NULL ) {
//        // We nest all the frames for lexical scoping. Any variable not bound locally, will
//        // be added as an implicit argument. These implicit arguments allows closures
//        // to refer to variables in their parent scope.
//        newMf->Parent = mf;
//    }

//    VALUE* registers = mf->compilationUnit->StartOfRegisters();
//    (*registers++) = NIL();
    //mf->AddConstant(NIL()); // Return value
    int cnt = lets.Count();
    
    for (int t=0;t<cnt;t += 2) {
        Symbol variableName = lets.GetAt(t).SymbolId;
        //std::cout << "Found r[" << varCount << "] as local variable " << lets.GetAt(t).Print() << "=" << lets.GetAt(t+1).Print() << "\n";
        //mf->LocalsAndArguments.push_back(Variable(variableName,form.GetAt(t+1)));
        mf->Registers.push_back(variableName);
        int regno = mf->AddConstant(lets.GetAt(t+1));
        //uint8_t regno = mf->Registers.size()-1;
        //mf->compilationUnit->AddInitializedRegister();
        mf->Bindings[variableName] = Binding(mf,regno);
        //(*registers++) = lets.GetAt(t+1);
    }
    //byte* bytecode = (byte*)registers;
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        std::cout << "Statements following let:" << form.First().Print() << "\n";
        CompileForm(isolate,mf,form.First(),UseReturnRegister);
        form = form.Rest();
    }
    
//    new (unit) CompilationUnit( code, registers,  );

    return 0;
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

byte* CompilePrototype( Isolate* isolate, Metaframe* mf, VALUE form ) {
    
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
    
    registers = r = header->StartOfRegisters();
    
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
    //*(c++) = Instruction(END);       // 3=Print 5=internadiate1
    
    //*(c++) = Instruction(JMP_IF_TRUE,(int32_t)-1);       // 3=Print 5=internadiate1
    uninitatedRegisters = 1;
    byte* p = (byte*)(c-1);
    
    header->sizeOfInitializedRegisters = ((byte*)code) - ((byte*)registers);
    header->sizeOfRegisters = header->sizeOfInitializedRegisters + uninitatedRegisters * sizeof(VALUE);
    
    return p;
}



int CompileConstant( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    // Compile a constant. If this is the last statement, it will
    // occupy the return register (r[0]).
    CompilationUnit* unit = mf->compilationUnit;
    if (mtd == UseReturnRegister) {
        unit->SetReturnRegister( form );
        return 0;
    }
    throw new std::runtime_error("Error");
}


int CompileSymbol( Isolate* isolate, Metaframe* mf, VALUE symbol, RegisterAllocationMethod mtd, bool deref ) {

    
    Instruction* i;
    
    int resultRegNo = mf->AllocateRegister(isolate, mtd, 0);
    
    //    mf->Bindings[symbol] ;asddsa
    auto x = mf->Bindings.find(symbol.SymbolId);
    
    if ( x == mf->Bindings.end()) {
        
//        throw std::runtime_error("Variable is not declared");
    
//        CompilationUnit* unit = mf->compilationUnit;
//        VALUE* registers = unit->;
        int regNo = mf->AddConstant( symbol );
        
        if (deref) {
           i = mf->BeginCodeWrite(isolate);
           //auto resultRegNo = (uint8_t)mf->AllocateIntermediateRegister();
           (*i++) = Instruction( DEREF, (uint8_t)resultRegNo, (uint8_t)regNo );
           mf->EndCodeWrite(i);
           return resultRegNo;
        }
        return regNo;
    }
    
    return x->second.Register;
//        throw std::runtime_error("Found local variable reference! Not implemented");
    
}



/*
void HonorResultRegister(Isolate* isolate, Metaframe* mf, int regNo) {
    if (regNo != 0) {
       Instruction* c = mf->BeginCodeWrite(isolate);
       (*c++) = Instruction(MOVE,(uint8_t)regNo,(uint8_t)0);
       mf->EndCodeWrite(c);
    }
}
 */


int CompileFnCall_SymbolOptimization( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    std::cout << "Function call: " << form.First().Print() << "\n";
    
    
    VALUE function = form.First();
    int usedBefore = mf->intermediatesUsed;
    int tmp;
    int argCount = form.Count() - 1;
    
    int regNo = mf->AllocateRegister(isolate, mtd, 0);
    
    for (int i=1;i<=argCount;i++) {
//        tmp = mf->AllocateIntermediateRegister(isolate);
        tmp = CompileForm(isolate, mf, form.GetAt(i), AllocatedRegister );
        isolate->MiniPush(tmp);
    }
    
    //tmp = mf->AllocateIntermediateRegister(isolate);
    tmp = CompileSymbol(isolate,mf,function,AllocatedRegister,false);
    // mf->FindBinding( function.Symbol );
    
    
    Instruction* i = mf->BeginCodeWrite(isolate);
    uint8_t a1,a2,a3,a4,a5;
    switch (argCount) {
        case 0:
            (*i++) = Instruction(SCALL_0,(uint8_t)regNo,(uint8_t)tmp);
            break;
        case 1:
            a1 = isolate->MiniPop();
            (*i++) = Instruction(SCALL_1,regNo,tmp,a1);
            break;
        case 2:
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            (*i++) = Instruction(SCALL_2,regNo,tmp,a1);
            (*i++) = Instruction(a2);
            break;
        case 3:
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            (*i++) = Instruction(SCALL_3,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3);
            break;
        case 4:
            a4 = isolate->MiniPop();
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            (*i++) = Instruction(SCALL_4,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3,a4);
            break;
        case 5:
            a5 = isolate->MiniPop();
            a4 = isolate->MiniPop();
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            (*i++) = Instruction(SCALL_5,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3,a4,a5);
            break;
        default:
            throw std::runtime_error("Not Implemented");
            break;
    }
    mf->EndCodeWrite(i);
    
    
    mf->FreeIntermediateRegisters(usedBefore);
    
   // HonorResultRegister(isolate,mf,regNo);
    
    return regNo;
}

int CompileParenthesis( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    if (form.IsEmpty()) {
        // () evaluates to an empty list whereas other lists are treated as/
        // function/procedure calls.
        return CompileConstant(isolate, mf, form, mtd );
    }
    VALUE function = form.First();
    if (function.IsSymbol()) {
        // This is a regular predefined function/procedure call such as (print 123)
        switch (function.SymbolId) {
            case (SymLetStar):
                return CompileLet( isolate, mf, form, mtd );
            case (SymFnStar):
                return CompileFn( isolate, mf, form, mtd );
            default:
                return CompileFnCall_SymbolOptimization( isolate, mf, form, mtd );
        }
        

    }
    // This is a direct lambda call such as ((fn [x] (print x)) 123)

    int usedBefore = mf->intermediatesUsed;
    std::cout << "Function call: " << form.First().Print() << "\n";
    int regNo = CompileForm(isolate,mf,function,mtd);
    Instruction* i = mf->BeginCodeWrite(isolate);
    (*i++) = OpCall(0);
    mf->EndCodeWrite(i);
    mf->FreeIntermediateRegisters(usedBefore);
    return regNo;
}

int CompileList( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    // TODO! We don't know that this list is a constant. Temporary code.
    return CompileConstant(isolate, mf, form, mtd );
}

/*
void AnalyseForm( Isolate* isolate, Metaframe* mf, VALUE form );


void AnalyseParenthesis( Isolate* isolate, Metaframe* mf, VALUE form ) {
    VALUE fn = form.First();
    if (!fn.IsSpecial()) {
        mf->FoundConstant(fn);
    }
    form = form.Rest();
    while (!form.IsEmptyList()) {
        AnalyseForm( isolate, mf, form.First());
        form = form.Rest();
    }
}

void AnalyseList( Isolate* isolate, Metaframe* mf, VALUE form ) {
    while (!form.IsEmptyList()) {
        AnalyseForm( isolate, mf, form.First());
        form = form.Rest();
    }
}

void AnalyseForm( Isolate* isolate, Metaframe* mf, VALUE form ) {
    switch (form.Type) {
        case TNumber:
            mf->FoundConstant(form);
            return;
        case TList:
            switch (form.ListStyle) {
                case QParenthesis:
                    AnalyseParenthesis( isolate, mf, form );
                    break;
                case QCurly:
                case QBracket:
                    AnalyseList( isolate, mf, form );
                    break;
                case QString:
                    mf->FoundConstant(form);
                    break;
            }
        case TAtom:
            switch (form.AtomSubType) {
                case ANil:
                case AKeyword:
                case ASymbol:
                    mf->FoundConstant(form);
                    break;
                case AOther:
                    break;
            }
        default:
            break;
    }
}
*/

int CompileForm( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    switch (form.Type) {
        case TNumber:
            return CompileConstant( isolate, mf, form, mtd );
        case TList:
            switch (form.ListStyle) {
                case QParenthesis:
                    return CompileParenthesis( isolate, mf, form, mtd );
                case QCurly:
                case QBracket:
                    return CompileList( isolate, mf, form, mtd );
                case QString:
                    return CompileConstant( isolate, mf, form, mtd );
            }
        case TAtom:
            switch (form.AtomSubType) {
                case ANil:
                case AKeyword:
                    return CompileConstant( isolate, mf, form, mtd );
                case ASymbol:
                    return CompileSymbol(isolate, mf, form, mtd, true );
                case AOther:
                    break;
            }
        default:
            break;
    }
    throw std::runtime_error("Compilation error");
}

Compilation* Compiler::Compile( Isolate* isolate, VALUE form ) {
    
    byte* p = (byte*)isolate->NextOnConstant;
    
    Metaframe* mf = MALLOC_HEAP(Metaframe); // TODO! STACKALLOC
    
    auto comp = new (p) Compilation();
    p += sizeof(Compilation);
    CompilationUnit* u = (CompilationUnit*)p;
    new (u) CompilationUnit();  //( Instruction* code, VALUE* registers, int sizeUninit )

    new (mf) Metaframe(NULL,u);
    
    mf->compilation = comp;
    mf->compilationUnit = u;
    
//    AnalyseForm( isolate, mf, form );
    CompileForm( isolate, mf, form, UseReturnRegister );
    
    comp->sizeOfCompilation = mf->FinishedWriting(isolate);
    
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
            case (DEREF):
                res << str;
                res << "       (r";
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
            case (SCALL_0):
            case (CALL_0):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (SCALL_1):
            case (CALL_1):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
                res << (int)p->C;
                res << ")";
                break;
            case (SCALL_2):
            case (CALL_2):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
                res << (int)p->C;
                p++;
                res << ",r";
                res << (int)p->OP;
                res << ")";
                break;
            case (SCALL_3):
            case (CALL_3):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
                res << (int)p->C;
                p++;
                res << ",r";
                res << (int)p->OP;
                res << ",r";
                res << (int)p->A;
                res << ")";
                break;
            case (SCALL_4):
            case (CALL_4):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
                res << (int)p->C;
                p++;
                res << ",r";
                res << (int)p->OP;
                res << ",r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (SCALL_5):
            case (CALL_5):
                res << str;
                res << "     (r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ",r";
                res << (int)p->C;
                p++;
                res << ",r";
                res << (int)p->OP;
                res << ",r";
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
