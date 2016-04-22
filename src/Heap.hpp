//
//  Heap.hpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB. All rights reserved.
//

#ifndef Heap_hpp
#define Heap_hpp

#include <stdio.h>
#include <cstdint>
#include <stdexcept>
#include <stdlib.h>
#include <iostream>

class Heap {
public:
    int NumberOfAllocations = 0;
    int BytesAllocated = 0;
    // Let's make use of the fact that Starcounter only operates on modern operating systems.
    // Our addresses must have the 4 least significant bits set to zero in order
    // to allow us to pack them in to our 64 bit value type. This is because our value type
    // uses 4 bits to declare types such that both primitive values and pointers can be hosted
    // in the same value structure. See Value.cpp.
    void* SafeMalloc( size_t size ) {
        uint64_t ret = (uint64_t)malloc(size);
        if ( ret & 0b0000000000000000000000000000000000000000000000000000000000001111 ) {
            throw std::runtime_error( "escape characters not implemented yet" );
            
        }
        BytesAllocated += size;
        NumberOfAllocations++;
        return (void*)ret;
    }
    void SafeFree( void* p ) {
        NumberOfAllocations--;
        free(p);
    }
    
    void PrintStatus() {
       if (NumberOfAllocations) {
           std::cout << "\nNumber of unfreed allocations: ";
           std::cout << NumberOfAllocations;
           std::cout << " (";
           std::cout << BytesAllocated;
           std::cout << " bytes) \n";
       }
    }
};

extern Heap Heap;

#endif /* Heap_hpp */
