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


class Array;

class ARRAY : public VALUE {
public:
    ARRAY() {
        Type = PList;
        Flag = true; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    ARRAY( Array* array ) {
        Integer = (uint64_t)array;
        Flag = true; // See VALUE::IsClassicalParenthesis
    }
    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    void Print();
    
    Array* GetArray() {
        return (class Array*)Integer;
    }
    
};


// Arrays are allocated on the heap with the VALUEs directly attached after the count variable.
// They are the cheapest form of lists until you detect that new lists are derived from the list.
// Because of this, the reader will always create lists of this type if the size of the list
// is at most 16 elements. If the list is larger than that, a Vector will be created instead.
// Cons pairs are almost never used unless the program explicitly does Cons operations.
class Array : public List {
public:
    uint32_t count; // The count of VALUE objects following this integer.
    // VALUE v1; VALUE v2....    
};



#endif /* Array_hpp */
