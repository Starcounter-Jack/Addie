//
//  Header.h
//  Addie
//
//  Created by Joachim Wester on 05/05/16.
//
//

#ifndef Compier_hpp
#define Compier_hpp

using namespace Addie::Internals;

namespace Addie {

    namespace Internals {
        
        class Metaframe;
        
        class Binding {
        public:
            Binding() {
                
            }
            Binding( Metaframe* frame, uint8_t reg ) : Frame(frame), Register(reg) {
            }
            
            Metaframe* Frame;
            uint8_t Register;
        };
        
        
        //struct Variable {
        //public:
        //    Variable( Symbol sym, VALUE val ) : symbol(sym), defaultValue(val) {
        //    }
        //    Symbol symbol;
        //    VALUE defaultValue;
        //};
        
        struct RegisterUse {
            bool InUse : 1;
        };
        
        
        class Metaframe {
        public:
            RegisterUse RegUsage[255] = { {false} };
            
            
            Metaframe( Metaframe* parent, CompilationUnit* unit ) :Parent(parent) {
                if (parent != NULL ){
                    compilation  = parent->compilation;
                    compilationUnit = parent->compilationUnit;
                }
                writeHead = ((byte*)unit) + sizeof(CompilationUnit) + sizeof(VALUE); // Skip return valueÏCo
                
            }
//            Metaframe( Metaframe* parent, CompilationUnit cu, Compilation c ) :Parent(parent),
//                                            compilationUnit(cu), compilation(c) {}
            Metaframe* Parent = NULL;
            
            std::vector<Symbol> Registers;
            std::map<Symbol,Binding> Bindings;
            
            VALUE Body;
//            int constants = 0;
            CompilationUnit* compilationUnit = NULL;
            Compilation* compilation = NULL;
            byte* writeHead;
            int intermediatesUsed = 0;
            int maxIntermediatesUsed = 0;
            
            Instruction* tempWriteHead;
            Instruction* tempBuffer = NULL; // Will point to a temporary stack allocation during compilation
            // size_t CodeBufferUsed = 0; // To pop (free) the temporary stack allocation during compilation
            
            
            int AllocateRegister( Isolate* isolate, RegisterAllocationMethod mtd, int existingRegNo ) {
                if (mtd == UseReturnRegister) {
                    return 0;
                }
                if (existingRegNo == 0) {
                    return AllocateIntermediateRegister(isolate);
                }
                return existingRegNo;
            }
            
            Instruction* BeginCodeWrite( Isolate* isolate ) {
                if (tempBuffer == NULL) {
                    tempWriteHead = tempBuffer = (Instruction*)(isolate->NextOnStack);
                }
                return tempWriteHead;
            }
            
            
            void EndCodeWrite( Instruction* addr ) {
                tempWriteHead = addr;
            }
            
//            void FoundConstant( VALUE form ) {
//                std::cout << "Found constant " << form.Print() << "\n";
//                constants++;
//            }

            
            int AllocateIntermediateRegister(Isolate* isolate) {
                intermediatesUsed++;
                if (intermediatesUsed > maxIntermediatesUsed)
                    maxIntermediatesUsed = intermediatesUsed;
                if (!RegUsage[0].InUse) {
                    RegUsage[0].InUse = true;
                    return 0;
                }
                int regNo = 255; // 6 - intermediatesUsed; // - 1;
                while (true) {
                    if (!RegUsage[regNo].InUse) {
                        RegUsage[regNo].InUse = true;
                        return regNo;
                    }
                    regNo--;
                }
                throw std::runtime_error("No free register");
            }
            
            void FreeIntermediateRegister( int regNo ) {
                RegUsage[regNo].InUse = false;
            }
            
            int AddConstant( VALUE value ) {
                
                int regNo;
                
                if (value.IsSymbol()) {
                    
                    Symbol id = value.SymbolId;
                    auto x = Bindings.find(id);
                    
                    if ( x != Bindings.end()) {
                        regNo = x->second.Register;
                        RegUsage[regNo].InUse = true;
                        return regNo;
                    }
                }
                
                std::cout << "Adding constant " << value.Print() << "\n";
                VALUE* reg = (VALUE*)writeHead;
                (*reg++) = value;
                writeHead = (byte*)reg;
                regNo = compilationUnit->AddInitializedRegister();
                RegUsage[regNo].InUse = true;
                
                if (value.IsSymbol()) {
                    Bindings[value.SymbolId] = Binding(this,regNo);
                    Registers[regNo] = value.SymbolId;
                }
                
                return regNo;
            }
            
            size_t FinishedWriting(Isolate* isolate) {
                // Copy the buffer into the compilation unit
                Instruction* c = BeginCodeWrite(isolate);
                (*c++) = Instruction(END);
                EndCodeWrite(c);
                int tempBufferUsed = ((byte*)tempWriteHead - (byte*)tempBuffer);
                if (tempBufferUsed != 0) {
                   memcpy( compilationUnit->StartOfInstructions(), tempBuffer, tempBufferUsed);
                }
                compilationUnit->ReportIntermediate(maxIntermediatesUsed);
                //return (byte*)((byte*)compilationUnit->StartOfInstructions() + tempBufferUsed);
                return sizeof(CompilationUnit) + compilationUnit->sizeOfInitializedRegisters +
                                        tempBufferUsed;
            }
        };

    }
    
    class Compiler {
    public:
        static Compilation* Compile( Isolate* isolate, VALUE form );
//        static Compilation* CompilePrototype( Isolate* isolate, VALUE form );
        static STRINGOLD Disassemble(  Isolate* isolate, Compilation* code );
    };
    

}


#endif /* Compier_hpp */
