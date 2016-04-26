//
//  Compiler.cpp
//  Addie
//
//  Created by Joachim Wester on 24/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Compiler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

//typedef int (*myfuncion)() abc;
typedef int (*AddZero)(float);

int JitX86( int num ) {
    // Machine code for:
    //   mov eax, 0
    //   ret
    unsigned char code[] = {0xb8, 0x00, 0x00, 0x00, 0x00, 0xc3};
    
    
    // Overwrite immediate value "0" in the instruction
    // with the user's value.  This will make our code:
    //   mov eax, <user's value>
    //   ret
    memcpy(&code[1], &num, 4);
    
    // Allocate writable/executable memory.
    // Note: real programs should not map memory both writable
    // and executable because it is a security risk.
    void *mem = mmap(NULL, sizeof(code), PROT_WRITE | PROT_EXEC,
                     MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(mem, code, sizeof(code));
    
    // The function will return the user's value./
    AddZero myfuncion;
    
    myfuncion = (AddZero)mem;
    
    return myfuncion(num);
}