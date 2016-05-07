//
//  Header.h
//  Addie
//
//  Created by Joachim Wester on 05/05/16.
//
//

#ifndef Compier_hpp
#define Compier_hpp

namespace Addie {
    namespace Internals {
        
class Compiler {
public:
    static Compilation* Compile( Isolate* isolate, VALUE form );
    static Compilation* CompilePrototype( Isolate* isolate, VALUE form );
    static STRINGOLD Disassemble(  Isolate* isolate, Compilation* code );
};

    }
}


#endif /* Compier_hpp */
