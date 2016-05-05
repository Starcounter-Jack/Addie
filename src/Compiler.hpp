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
    static Compilation* Compile( VALUE form );
    static Compilation* CompilePrototype( VALUE form );
    static STRINGOLD Disassemble( Compilation* code );
};

    }
}


#endif /* Compier_hpp */
