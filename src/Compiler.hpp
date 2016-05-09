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
        
        //struct Variable {
        //public:
        //    Variable( Symbol sym, VALUE val ) : symbol(sym), defaultValue(val) {
        //    }
        //    Symbol symbol;
        //    VALUE defaultValue;
        //};
        
        class Metaframe {
        public:
            std::vector<Symbol> LocalsAndArguments;
            std::map<Symbol,uint8_t> Bindings;
            VALUE Body;
        };

    }
    
    class Compiler {
    public:
        static Metaframe* Analyse( Isolate* isolate, Namespace* ns, VALUE form );
        static Compilation* Compile( Isolate* isolate, Namespace* ns, VALUE form );
        static Compilation* CompilePrototype( Isolate* isolate, VALUE form );
        static STRINGOLD Disassemble(  Isolate* isolate, Compilation* code );
    };
    

}


#endif /* Compier_hpp */
