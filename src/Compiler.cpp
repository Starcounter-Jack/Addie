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


void CompileForm( Isolate* isolate, Metaframe* mf, VALUE form, int regNo );



// Compile a function/lambda declaration
void CompileFn( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    
    
    VALUE args = form.Rest().First();
    
    int cnt = args.Count();
    
    for (int t=0;t<cnt;t++) {
        Symbol argName = args.GetAt(t).SymbolId;
        mf->Registers.push_back(argName);
        //int regno = mf->AddConstant(NIL());
        mf->Bindings[argName] = Binding(mf,regNo);
    }
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        std::cout << "Statements following fn:" << form.First().Print() << "\n";
        CompileForm(isolate,mf,form.First(),0);
        form = form.Rest();
    }
    
    //    new (unit) CompilationUnit( code, registers,  );
    
    return;
}


// Find all declared variables and their default values
void CompileLet( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {

    
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
        CompileForm(isolate,mf,form.First(),0);
        form = form.Rest();
    }
    
//    new (unit) CompilationUnit( code, registers,  );

    return;
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



void CompileConstant( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    // Compile a constant. If this is the last statement, it will
    // occupy the return register (r[0]).
    CompilationUnit* unit = mf->compilationUnit;
    unit->SetReturnRegister( form );
}

int CompileSymbol( Isolate* isolate, Metaframe* mf, VALUE symbol, int resultRegNo ) {

    
    Instruction* i;
    
    //    mf->Bindings[symbol] ;asddsa
    auto x = mf->Bindings.find(symbol.SymbolId);
    
    if (true || x == mf->Bindings.end()) {
        
//        throw std::runtime_error("Variable is not declared");
    
//        CompilationUnit* unit = mf->compilationUnit;
//        VALUE* registers = unit->;
        int regNo = mf->AddConstant( symbol );
        
        i = mf->BeginCodeWrite(isolate);
        //auto resultRegNo = (uint8_t)mf->AllocateIntermediateRegister();
        (*i++) = Instruction( DEREF, (uint8_t)resultRegNo, (uint8_t)regNo );
        mf->EndCodeWrite(i);
        return resultRegNo;
    }
        throw std::runtime_error("Found local variable reference! Not implemented");
    
}

void HonorResultRegister(Isolate* isolate, Metaframe* mf, int regNo) {
    if (regNo != 0) {
       Instruction* c = mf->BeginCodeWrite(isolate);
       (*c++) = Instruction(MOVE,(uint8_t)regNo,(uint8_t)0);
       mf->EndCodeWrite(c);
    }
}


void CompileFnCall( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    std::cout << "Function call: " << form.First().Print() << "\n";
    VALUE function = form.First();
    int usedBefore = mf->intermediatesUsed;
    int tmp;
    int argCount = form.Count() - 1;
    
    
    for (int i=1;i<=argCount;i++) {
        tmp = mf->AllocateIntermediateRegister(isolate);
        isolate->MiniPush(tmp);
//        std::cout << "Preparing argument " << tmp << "\n";
        CompileForm(isolate, mf, form.GetAt(i), tmp);
//        Instruction* c = mf->BeginCodeWrite(isolate);
//        (*c++) = Instruction(MOVE,(uint8_t)0,(uint8_t)tmp);
//        mf->EndCodeWrite(c);
    }
    
    tmp = mf->AllocateIntermediateRegister(isolate);
    CompileSymbol(isolate,mf,function,tmp);
    // mf->FindBinding( function.Symbol );
    Instruction* i = mf->BeginCodeWrite(isolate);
    uint8_t a1,a2;
    switch (argCount) {
        case 0:
            (*i++) = OpCall(tmp);
            break;
        case 1:
            a1 = isolate->MiniPop();
            (*i++) = OpCall(tmp,a1);
            break;
        case 2:
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            (*i++) = OpCall(tmp,a1,a2);
            break;
        default:
            throw std::runtime_error("Not Implemented");
            break;
    }
    mf->EndCodeWrite(i);
    
    
    mf->FreeIntermediateRegisters(usedBefore);
    
    HonorResultRegister(isolate,mf,regNo);
    
    return;
}

void CompileParenthesis( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    if (form.IsEmpty()) {
        // () evaluates to an empty list whereas other lists are treated as/
        // function/procedure calls.
        CompileConstant(isolate, mf, form, 0 );
        return;
    }
    VALUE function = form.First();
    if (function.IsSymbol()) {
        // This is a regular predefined function/procedure call such as (print 123)
        switch (function.SymbolId) {
            case (SymLetStar):
                return CompileLet( isolate, mf, form, regNo );
            case (SymFnStar):
                return CompileFn( isolate, mf, form, regNo );
            default:
                return CompileFnCall( isolate, mf, form, regNo );
        }
        

    }
    // This is a direct lambda call such as ((fn [x] (print x)) 123)

    int usedBefore = mf->intermediatesUsed;
    std::cout << "Function call: " << form.First().Print() << "\n";
    CompileForm(isolate,mf,function,0);
    Instruction* i = mf->BeginCodeWrite(isolate);
    (*i++) = OpCall(0);
    mf->EndCodeWrite(i);
    mf->FreeIntermediateRegisters(usedBefore);
    return;
}

void CompileList( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    // TODO! We don't know that this list is a constant. Temporary code.
    CompileConstant(isolate, mf, form, regNo );
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

void CompileForm( Isolate* isolate, Metaframe* mf, VALUE form, int regNo ) {
    switch (form.Type) {
        case TNumber:
            CompileConstant( isolate, mf, form, regNo );
            return;
        case TList:
            switch (form.ListStyle) {
                case QParenthesis:
                    CompileParenthesis( isolate, mf, form, regNo );
                    return;
                case QCurly:
                case QBracket:
                    CompileList( isolate, mf, form, regNo );
                    return;
                case QString:
                    CompileConstant( isolate, mf, form, regNo );
                    return;
            }
        case TAtom:
            switch (form.AtomSubType) {
                case ANil:
                case AKeyword:
                    CompileConstant( isolate, mf, form, regNo );
                    return;
                case ASymbol:
                    CompileSymbol(isolate, mf, form, regNo );
                    return;
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
    CompileForm( isolate, mf, form, 0 );
    
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
