//
//  Compiler.hpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Compiler_hpp
#define Compiler_hpp

#include <stdio.h>
#include <vector>
#include "Value.hpp"

enum Codes {
    PUSH = 123,
    POP = 124
};


class Context {
public:
    std::vector<uint8_t> ByteCode;
    std::vector<uint8_t> DebugCode;    
};


class Compiler {
    
    
public:
    
    STRING Compile( VALUE form ) {
        Context c;
        c.ByteCode.push_back( PUSH );
        c.ByteCode.push_back( POP );
        
        std::cout << "Byte code:";
        std::cout << (int)c.ByteCode[0] << ".";
        std::cout << (int)c.ByteCode[1];
        
        return STRING( (char*)c.ByteCode.data(), 2);
    }
};

#endif /* Compiler_hpp */
