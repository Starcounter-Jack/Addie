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
#include "Isolate.hpp"

extern int JitX86( int num );

typedef uint8_t byte;


class Context {
public:
    std::vector<byte> ByteCode;
    std::vector<byte> DebugCode;
};


class Compiler {
    
    
public:
    
    STRING Compile( VALUE form ) {

 
        
        Context c;
        c.ByteCode.push_back( MOVE_aa );
        c.ByteCode.push_back( MOVE_rr );
        
        std::cout << "Byte code:";
        std::cout << (int)c.ByteCode[0] << ".";
        std::cout << (int)c.ByteCode[1];
        
        return STRING( (char*)c.ByteCode.data(), 2);
    }
};

#endif /* Compiler_hpp */
