//
//  Addie
//
//  Array.hpp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Array_hpp
#define Array_hpp

#include "Array.hpp"
#include "Value.hpp"
#include "Cons.hpp"


#include "Isolate.hpp"

enum ArrayElementType {
    Any = 0b0,
    IntegersOnly = 0b1,
};

struct ArrayHeader {
    ArrayElementType Type : 1;      // Important for efficiency for things like strings
    unsigned int ElementSize : 4;   // Important for efficiency for things like strings
    unsigned int Count : 27;
};

// Arrays are allocated on the heap with the values directly attached after the count variable.
// They are the cheapest form of lists until you detect that new lists are derived from the list.
// Because of this, the reader will always create lists of this type if the size of the list
// is at most 16 elements. If the list is larger than that, a Vector will be created instead.
// Cons pairs are almost never used unless the program explicitly does Cons operations.
template <class T>
class Array : public List {
public:
    ArrayHeader Header;
    // T Values[n];
    
    static void BeginWrite() {
        *((ArrayHeader*)CurrentIsolate->NextOnHeap) = ArrayHeader { Any, 8, 0 };
        CurrentIsolate->NextOnHeap += sizeof(ArrayHeader);
    }
    
    void Write( T v ) {
        (*((T*)CurrentIsolate->NextOnHeap)) = v;
        CurrentIsolate->NextOnHeap += sizeof(T);
    }

};



#endif /* Array_hpp */
