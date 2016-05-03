//
//  Array.hpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Array_hpp
#define Array_hpp

#include "Array.hpp"
#include "Value.hpp"
#include "Cons.hpp"


#include "Isolate.hpp"

#define ONLY_USE_RAW_ARRAYS
                // For tracking down bugs, we can create a naive version of Addie.
                // If defined, we will always create fresh new arrays instead of using the
                // persistent vector optimizations.

enum ArrayElementType {
    Any = 0b0,           // Each element can be of any Addie type
    IntegersOnly = 0b1,  // Use this one for strings or integer arrays
};

struct ArrayHeader {
    ArrayElementType Type : 1;      // Important for efficiency for things like strings
    unsigned int ElementSize : 4;   // Important for efficiency for things like strings
    unsigned int Count : 27; // The number of elements in the list.
};

// Arrays are allocated on the heap with the values directly attached after the count variable.
// They are the cheapest form of lists until you detect that new lists are derived from the list.
// Because of this, the reader will always create lists of this type if the size of the list
// is at most 16 elements. If the list is larger than that, a Vector will be created instead.
// Cons pairs are almost never used unless the program explicitly does Cons operations.
template <class T>
class Array : public List {
public:
    Array() {
        Header.Type = Any;
        Header.ElementSize = sizeof(T);
        Header.Count = 0;
    }
    
    ArrayHeader Header;
    // Will be followed by: T Values[n];
    
    // Let's just cooperate with the memory allocator to make things REALLY fast.
    static Array* __beginWrite() {
        Array* arr = (Array*)CurrentIsolate->NextOnHeap;
        new (arr) Array();
//        *((ArrayHeader*)arr) = ArrayHeader { Any, 8, 0 };
        CurrentIsolate->NextOnHeap += sizeof(Array);
        return (arr);
    }
    
    // It is only legal to call __write following __beginWrite given that
    // there are NO other allocations being done inbetween.
    void __write( T v ) {
        (*((T*)CurrentIsolate->NextOnHeap)) = v;
        CurrentIsolate->NextOnHeap += sizeof(T);
        Header.Count++;
    }
    
    void __endWrite() {
        CurrentIsolate->AlignNextHeap();
    }
    
    // The actual elements are stored immediately after the header
    VALUE* __values() {
        return (VALUE*)(((uint8_t*)this) + sizeof(Array));
    }
    
    // Override of the List interface
    VALUE First() {
        throw std::runtime_error("Not implemented yet");
    }
    
    // Override of the List interface
    VALUE Rest() {
        throw std::runtime_error("Not implemented yet");
    }
    
    // Override of the List interface
    int Count() {
        return Header.Count;
    }

    
    // Override of the List interface
    VALUE GetAt( int i ) {
        assert( i < Header.Count );
        return __values()[i];
    }
    
    // Override of the List interface
    List* ReplaceAt( int i, VALUE v ) {
        assert( i < Header.Count );
        if (RefCount == 0) {
            __values()[i] = v;
            return this;
        }
        
#ifdef ONLY_USE_RAW_ARRAYS
        // Make a copy of the list
        size_t size = sizeof(Array) + Header.Count * sizeof(T);
        Array* newList = (Array*)CurrentIsolate->NextOnHeap;
        memcpy( newList, this, size );
        CurrentIsolate->ReportHeapWrite(size);
        newList->__values()[i] = v;
        return newList;
#else
        // Upgrade to a bitmapped vector trie as it is faster for
        // deriving new lists.
        throw std::runtime_error("Not implemented yet");
#endif
    }
    
    // Override of the List interface
    List* Append( VALUE v ) {
#ifdef ONLY_USE_RAW_ARRAYS
        // Make a copy of the list
        size_t size = sizeof(Array) + Header.Count * sizeof(T);
        Array* newList = (Array*)CurrentIsolate->NextOnHeap;
        memcpy( newList, this, size );
        __values()[newList->Header.Count] = v;
        newList->Header.Count++;
        CurrentIsolate->ReportHeapWrite(size + sizeof(T));

        return newList;
#else
        // Upgrade to a bitmapped vector trie as it is faster for
        // deriving new lists.
        throw std::runtime_error("Not implemented yet");
#endif
    }
    
    
    // Override of the List interface
    List* Prepend( VALUE v ) {
#ifdef ONLY_USE_RAW_ARRAYS
        // Make a copy of the list
        size_t elemsize = Header.Count * sizeof(T);
        byte* newList = (byte*)CurrentIsolate->NextOnHeap;
        memcpy( newList, this, sizeof(Array) );
        memcpy( newList + sizeof(Array) + sizeof(T), this, elemsize );
        __values()[0] = v;
        CurrentIsolate->ReportHeapWrite(sizeof(Array) + elemsize + sizeof(T));
        ((Array*)newList)->Header.Count++;
        return (Array*)newList;
#else
        // Upgrade to a bitmapped vector trie as it is faster for
        // deriving new lists.
        throw std::runtime_error("Not implemented yet");
#endif
    }

    List* RemoveAt( int i ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    List* InsertAt( int i, VALUE v ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Concatenate( VALUE v ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Reverse() {
        throw std::runtime_error("Not implemented yet");
    }

    List* Replace( VALUE v1, VALUE v2 ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Sort( VALUE fun ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Map( VALUE fun ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* First( int i ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Last( int i ) {
        throw std::runtime_error("Not implemented yet");
    }

    List* Skip( int i ) {
        throw std::runtime_error("Not implemented yet");
    }

};



#endif /* Array_hpp */
