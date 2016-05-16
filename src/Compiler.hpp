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
        
        class Metaframe {
        public:
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
            CompilationUnit* compilationUnit = NULL;
            Compilation* compilation = NULL;
            byte* writeHead;
            
            Instruction* tempWriteHead;
            Instruction* tempBuffer = NULL; // Will point to a temporary stack allocation during compilation
            // size_t CodeBufferUsed = 0; // To pop (free) the temporary stack allocation during compilation
            
            
            Instruction* BeginCodeWrite( Isolate* isolate ) {
                if (tempBuffer == NULL) {
                    tempWriteHead = tempBuffer = (Instruction*)(isolate->NextOnStack);
                }
                return tempWriteHead;
            }
            
            void EndCodeWrite( Instruction* addr ) {
                tempWriteHead = addr;
            }
            
            int AddConstant( VALUE value ) {
                std::cout << "Adding constant " << value.Print() << "\n";
                VALUE* reg = (VALUE*)writeHead;
                (*reg++) = value;
                writeHead = (byte*)reg;
                compilationUnit->AddInitializedRegister();
                return compilationUnit->GetInitializedRegisterCount() - 1;
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
                //return (byte*)((byte*)compilationUnit->StartOfInstructions() + tempBufferUsed);
                return sizeof(CompilationUnit) + compilationUnit->SizeOfInitializedRegisters +
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
