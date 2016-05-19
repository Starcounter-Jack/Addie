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
int CompileFn( Isolate* isolate, Metaframe* oldMf, VALUE form, RegisterAllocationMethod mtd ) {
    
    assert( mtd == UseReturnRegister);

    Instruction* c = oldMf->BeginCodeWrite(isolate);
    (*c++) = Instruction(ENCLOSE_0,(uint8_t)0,(uint8_t)0);
    oldMf->EndCodeWrite(isolate,c);

//    CodeFrame* newCodeFrame = (CodeFrame*)oldMf->writeHead;
    //new (newCodeFrame) CodeFrame();
    
    Metaframe* newFrame = MALLOC_HEAP(Metaframe); // TODO! GC
    new (newFrame) Metaframe(isolate,oldMf->currentScope,oldMf->compilation);

//    Metaframe* newFrame = oldMf;
    
    VALUE args = form.Rest().First();
    
    int cnt = args.Count();
    
    for (int t=0;t<cnt;t++) {
        Symbol argName = args.GetAt(t).SymbolId;
        int regNo = newFrame->currentScope->AllocateInitializedRegister(isolate,NIL(),argName);
        newFrame->RegUsage[regNo].IsArgument = true;
    }
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        //std::cout << "Statements following fn:" << form.First().Print() << "\n";
        CompileForm( isolate,newFrame,form.First(), UseReturnRegister );
        form = form.Rest();
    }
    
    //    new (unit) CodeFrame( code, registers,  );
    newFrame->Seal(isolate);
    
    return 0;
}


// Find all declared variables and their default values
int CompileLet( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {

    assert( mtd == UseReturnRegister );
    
    VALUE lets = form.Rest().First();

    VariableScope* scope = MALLOC_HEAP(VariableScope); // TODO! GC
    new (scope) VariableScope();
    scope->metaframe = mf;
    scope->parent = mf->currentScope;
    mf->currentScope = scope;
    
//    auto newMf = MALLOC_HEAP(Metaframe); // TODO! GC
//    new (newMf) Metaframe( mf );

//    if (mf != NULL ) {
//        // We nest all the frames for lexical scoping. Any variable not bound locally, will
//        // be added as an implicit argument. These implicit arguments allows closures
//        // to refer to variables in their parent scope.
//        newMf->Parent = mf;
//    }

//    VALUE* registers = mf->codeFrame->StartOfRegisters();
//    (*registers++) = NIL();
    //mf->AllocateConstant(NIL()); // Return value
    int cnt = lets.Count();
    
    for (int t=0;t<cnt;t += 2) {
        Symbol variableName = lets.GetAt(t).SymbolId;
        //mf->currentScope->Registers.push_back(variableName);
        mf->currentScope->AllocateInitializedRegister(isolate,lets.GetAt(t+1),variableName);
//        mf->currentScope->BindSymbolToRegister(variableName, regno);
//        mf->RegUsage[regno].InUse = true;
//        mf->RegUsage[regno].IsConstant = true;
    }
    //byte* bytecode = (byte*)registers;
    
    form = form.Rest().Rest();
    while (!form.IsEmptyList()) {
        //std::cout << "Statements following let:" << form.First().Print() << "\n";
        CompileForm(isolate,mf,form.First(),UseReturnRegister);
        form = form.Rest();
    }
    
//    new (unit) CodeFrame( code, registers,  );

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





int CompileConstant( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    // Compile a constant. If this is the last statement, it will
    // occupy the return register (r[0]).
    CodeFrame* unit = mf->codeFrame;
    if (mtd == UseReturnRegister) {
        mf->SetReturnRegister( form );
        return 0;
    }
    throw new std::runtime_error("Error");
}


int CompileSymbol( Isolate* isolate, Metaframe* mf, VALUE symbol, RegisterAllocationMethod mtd, bool deref ) {

    
    Instruction* i;
    
    
    //    mf->Bindings[symbol] ;asddsa
    int x = mf->currentScope->FindRegisterForSymbol(symbol.SymbolId);
    
    if ( x == -1 ) {
        
//        throw std::runtime_error("Variable is not declared");
    
//        CodeFrame* unit = mf->codeFrame;
//        VALUE* registers = unit->;
        //int regNo = mf->AllocateConstant( symbol );
        int regNo = mf->currentScope->AllocateInitializedRegister(isolate,symbol, symbol.SymbolId);
        
        if (deref) {
            int resultRegNo = mf->AllocateRegister(isolate, mtd, 0);
           i = mf->BeginCodeWrite(isolate);
           //auto resultRegNo = (uint8_t)mf->AllocateIntermediateRegister();
           (*i++) = Instruction( DEREF, (uint8_t)resultRegNo, (uint8_t)regNo );
           mf->EndCodeWrite(isolate,i);
           return resultRegNo;
        }
        return regNo;
    }
    
    return x;
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



int CompileFnCall( Isolate* isolate, Metaframe* mf, VALUE form, RegisterAllocationMethod mtd ) {
    //std::cout << "Function call: " << form.First().Print() << "\n";
    
    
    VALUE function = form.First();
    int tmp;
    int argCount = form.Count() - 1;
    bool isSymbol = function.IsSymbol();
    
    int regNo;
    
    //int usedBefore = mf->intermediatesUsed;
    
    for (int i=1;i<=argCount;i++) {
        //        tmp = mf->AllocateIntermediateRegister(isolate);
        tmp = CompileForm(isolate, mf, form.GetAt(i), UseFree );
        isolate->MiniPush(tmp);
    }

    Op op;
    
#ifdef USE_COMBINED_VM_OPS
    if (isSymbol) {
        // Optimization such that symbol dereferencing is done
        // in the same cycle as the call.
        tmp = CompileSymbol(isolate,mf,function,UseFree,false);
        op = SCALL_0;
    }
    else {
#endif
        tmp = CompileForm(isolate,mf,function,UseFree);
        op = CALL_0;
#ifdef USE_COMBINED_VM_OPS
    }
#endif
    
    
    Instruction* i = mf->BeginCodeWrite(isolate);
    uint8_t a1,a2,a3,a4,a5;
    switch (argCount) {
        case 0:
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op,(uint8_t)regNo,(uint8_t)tmp);
            break;
        case 1:
            a1 = isolate->MiniPop();
            mf->FreeIntermediateRegister(a1);
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op+1,regNo,tmp,a1);
            break;
        case 2:
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            mf->FreeIntermediateRegister(a1);
            mf->FreeIntermediateRegister(a2);
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op+2,regNo,tmp,a1);
            (*i++) = Instruction(a2);
            break;
        case 3:
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            mf->FreeIntermediateRegister(a1);
            mf->FreeIntermediateRegister(a2);
            mf->FreeIntermediateRegister(a3);
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op+3,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3);
            break;
        case 4:
            a4 = isolate->MiniPop();
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            mf->FreeIntermediateRegister(a1);
            mf->FreeIntermediateRegister(a2);
            mf->FreeIntermediateRegister(a3);
            mf->FreeIntermediateRegister(a4);
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op+4,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3,a4);
            break;
        case 5:
            a5 = isolate->MiniPop();
            a4 = isolate->MiniPop();
            a3 = isolate->MiniPop();
            a2 = isolate->MiniPop();
            a1 = isolate->MiniPop();
            mf->FreeIntermediateRegister(a1);
            mf->FreeIntermediateRegister(a2);
            mf->FreeIntermediateRegister(a3);
            mf->FreeIntermediateRegister(a4);
            mf->FreeIntermediateRegister(a5);
            regNo = mf->AllocateRegister(isolate, mtd, 0);
            (*i++) = Instruction(op+5,regNo,tmp,a1);
            (*i++) = Instruction(a2,a3,a4,a5);
            break;
        default:
            throw std::runtime_error("Not Implemented");
            break;
    }
    mf->EndCodeWrite(isolate,i);
    
    
    // mf->FreeIntermediateRegisters(usedBefore);
    
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
        }
        

    }
    return CompileFnCall( isolate, mf, form, mtd );
    
//    throw std::runtime_error("Not implemented");

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

// Relocate register
inline void Pack( uint8_t &reg, int lastFixed ) {
    if (reg > lastFixed) {
        // This is an intermediate result register. Let's use the register space
        // adjacent to the prematererialized registers (constants/locals/arguments)
        int tmp = -reg + 255;
        reg = tmp + lastFixed + 1;
    }
}

// Pack registers such that there is no unused registers
// inbetween constants/arguments/locals and intermediate registers
void PackRegisters( Isolate* isolate, Metaframe* mf ) {
    
    
    //CodeFrame* code = mf->codeFrame;
    Instruction* p = mf->tempCodeBuffer;
    
    int lastFixed = mf->maxInitializedRegisters - 1;
    
    while (true) {
        
        switch (p->OP) {
            case (RET):
                goto end;
            case (SCALL_0):
            case (CALL_0):
            case (MOVE):
            case (DEREF):
            case (ENCLOSE_0):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                break;
            case (SCALL_1):
            case (CALL_1):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                break;
            case (SCALL_2):
            case (CALL_2):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                p++;
                Pack(p->OP, lastFixed);
                break;
            case (SCALL_3):
            case (CALL_3):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                p++;
                Pack(p->OP, lastFixed);
                Pack(p->A, lastFixed);
                break;
            case (SCALL_4):
            case (CALL_4):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                p++;
                Pack(p->OP, lastFixed);
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                break;
            case (SCALL_5):
            case (CALL_5):
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                p++;
                Pack(p->OP, lastFixed);
                Pack(p->A, lastFixed);
                Pack(p->B, lastFixed);
                Pack(p->C, lastFixed);
                break;
            default:
                throw std::runtime_error("Not implemented");
                break;
        }
        p++;
    }
end:
    return;
    //return (uintptr_t)p;
}


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
    
    
    assert( isolate->NextOnStack == isolate->Stack ); // Check of memory leaks
    assert( isolate->NextOnStack2 == isolate->Stack2 ); // Check of memory leaks

    byte* p = (byte*)isolate->NextOnConstant;
    
    Metaframe* mf = MALLOC_HEAP(Metaframe); // TODO! STACKALLOC
    
    auto comp = new (p) Compilation();
    //p += sizeof(Compilation);
//    CodeFrame* u = (CodeFrame*)p;
//    new (u) CodeFrame();  //( Instruction* code, VALUE* registers, int sizeUninit )

    new (mf) Metaframe(isolate,NULL,comp);
    
//    mf->compilation = comp;
//    mf->codeFrame = u;
//    u->metaframe = mf;
    
    
//    AnalyseForm( isolate, mf, form );
    CompileForm( isolate, mf, form, UseReturnRegister );
    //comp->sizeOfCompilation += mf->FinishedWriting(isolate);
    mf->Seal(isolate);
    
//    PackRegisters(isolate, mf);
    isolate->ReportConstantWrite( (uintptr_t)comp->GetWriteHead() ); // Mark the memory as used

    assert( isolate->NextOnStack == isolate->Stack ); // Check of memory leaks
    assert( isolate->NextOnStack2 == isolate->Stack2 ); // Check of memory leaks
    
    return comp;
}


void Indent( std::ostringstream& res, std::string str ) {
    int prefix = str.length();
    while (prefix++ < 12) res << " ";

}


uintptr_t DisassembleUnit( Isolate* isolate, std::ostringstream& res, CodeFrame* code ) {
    
    Metaframe* mf = code->metaframe;
    
    const char* str;
    VALUE* R = code->StartOfRegisters();
    
    int prefix = 6;
    res << "============================================================\n";
    res << "START:";
    
    
    Instruction* p = code->StartOfInstructions();
    byte* eos = (byte*)code + mf->sizeOfCodeFrame; // - code->sizeOfInitializedRegisters;
    Instruction* end = (Instruction*)eos;
    
    while (p < end) {
        
        while (prefix++ < 12) res << " ";
        
        res << ".";
        str = isolate->GetStringFromSymbolId((*p).OP).c_str();
        
        switch (p->OP) {
            case (RET):
                res << str;
                break;
                //goto end;
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
                Indent( res, str );
                res << "(r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;

            case (MOVE):
                res << str;
                Indent( res, str );
                res << "(r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (SCALL_0):
            case (CALL_0):
                res << str;
                Indent( res, str );
                res << "(r";
                res << (int)p->A;
                res << ",r";
                res << (int)p->B;
                res << ")";
                break;
            case (SCALL_1):
            case (CALL_1):
                res << str;
                Indent( res, str );
                res << "(r";
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
                Indent( res, str );
                res << "(r";
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
                Indent( res, str );
                res << "(r";
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
                Indent( res, str );
                res << "(r";
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
    int regCount = mf->maxInitializedRegisters;
    res << "\n------------------------------------------------------------\n";
    for (int t=0;t<regCount;t++) {
        res << "r";
        res << t;
        res << " (";
        res << SYMBOL(mf->ExplainRegister(t)).Print();
        res << "):     ";
        res << R[t].Print();
        res << "\n";
    }
//    res << "============================================================\n";
    
    //std::cout << "End OP" << (int)p->OP << "\n";
    //assert( p->OP == 123);
    
    return (uintptr_t)p;
}

STRINGOLD Compiler::Disassemble( Isolate* isolate, Compilation* compilation ) {
    
    CodeFrame* code = compilation->GetFirstCodeFrame();
    std::ostringstream res;

    uintptr_t p = (uintptr_t)code;
    uintptr_t end = (uintptr_t)compilation->GetWriteHead() - 1;
    while (p < end ) {
        auto cf = (CodeFrame*)p;
       DisassembleUnit( isolate, res, cf );
       p += cf->metaframe->sizeOfCodeFrame; // TODO! ERROR! WHY?????
    }
    
    res << "============================================================\n";

    
    return STRINGOLD(res.str());

}


void VariableScope::BindSymbolToRegister( Symbol id, int regNo ) {
    Bindings[id] = Binding(this->metaframe,regNo);
    if (metaframe->Registers.size()<=regNo) {
        assert( metaframe->Registers.size() == regNo );
        metaframe->Registers.push_back(id);
    }
    else {
        metaframe->Registers[regNo] = id;
    }
}

int VariableScope::AllocateInitializedRegister( Isolate* isolate, VALUE value, Symbol symbol ) {
    int regNo = metaframe->__allocateConstant(isolate,value);
    metaframe->currentScope->BindSymbolToRegister(symbol, regNo);
    assert( metaframe->currentScope->FindRegisterForSymbol(symbol) == regNo);
    metaframe->RegUsage[regNo].InUse = true;
    metaframe->RegUsage[regNo].IsConstant = true;
    
    //std::cout << "Allocating register " << regNo << " to mean " << SYMBOL(symbol).Print() << "\n";
    
    return regNo;
}

void Metaframe::Seal(Isolate* isolate) {
    // Copy the buffer into the compilation unit
    
    
    Instruction* c = BeginCodeWrite(isolate);
    (*c++) = Instruction(RET);
    //(*c++) = Instruction(123);
    EndCodeWrite(isolate,c);
    
    PackRegisters(isolate, this);

    //codeFrame->SealIntermediate(maxIntermediatesUsed);
    
    int registersUsed = maxInitializedRegisters + maxIntermediatesUsed;
    assert( codeFrame == NULL );
    codeFrame = (CodeFrame*)compilation->GetWriteHead();
    new (codeFrame) CodeFrame( this, 0, registersUsed, maxInitializedRegisters);
    
    int tempRegisterBufferUsed = ((byte*)tempRegisterWriteHead - (byte*)tempRegisterBuffer);
    if (tempRegisterBufferUsed != 0) {
        memcpy( codeFrame->StartOfRegisters(), tempRegisterBuffer, tempRegisterBufferUsed);
        isolate->PopStack2(tempRegisterBufferUsed);
    }
    
    int tempCodeBufferUsed = ((byte*)tempCodeWriteHead - (byte*)tempCodeBuffer);
    if (tempCodeBufferUsed != 0) {
        memcpy( codeFrame->StartOfInstructions(), tempCodeBuffer, tempCodeBufferUsed);
        isolate->PopStack(tempCodeBufferUsed);
    }
    
    //return (byte*)((byte*)codeFrame->StartOfInstructions() + tempBufferUsed);
    size_t written = sizeof(CodeFrame) + codeFrame->sizeOfInitializedRegisters +
                            tempCodeBufferUsed;
    sizeOfCodeFrame = written;
    compilation->sizeOfCompilation += written;
    

    /*
    std::cout << "CodeFrame: " << (uintptr_t)codeFrame << "\n";
    std::cout << "Start-of-regs: " << (uintptr_t)codeFrame->StartOfRegisters() << "\n";
    std::cout << "Start-of-code: " << (uintptr_t)codeFrame->StartOfInstructions() << "\n";
    std::cout << "Register initializations: " << codeFrame->sizeOfInitializedRegisters/sizeof(VALUE) << "\n";
    std::cout << "Instructions written: " << tempCodeBufferUsed/sizeof(Instruction) << "\n";
     */
}



