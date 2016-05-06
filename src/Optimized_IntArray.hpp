//
//  IntArray.hpp
//  Addie
//
//  This code is optional. It will only be used if you compile with USE_OPTIMIZATIONS.
//  Skip this when you first want to learn about working with the Addie codebase.
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifdef USE_INTARRAY

#ifndef IntArray_hpp
#define IntArray_hpp

#include "VM.hpp"
#include "Optimized_Array.hpp"


namespace Addie {
    namespace Internals {

struct ArrayHeader {
    unsigned int ElementSize : 4;   // Important for efficiency for things like strings
    unsigned int Count : 28; // The number of elements in the list.
};


// Arrays are allocated on the heap with the values directly attached after the count variable.
// They are the cheapest form of lists until you detect that new lists are derived from the list.
// Because of this, the reader will always create lists of this type if the size of the list
// is at most 16 elements. If the list is larger than that, a Vector will be created instead.
// Cons pairs are almost never used unless the program explicitly does Cons operations.
template <class T>
class IntArray : public List {
public:
    IntArray() {
        Header.ElementSize = sizeof(T);
        Header.Count = 0;
    }
    
    ArrayHeader Header;
    // Will be followed by: T Values[n];
    
    // Let's just cooperate with the memory allocator to make things REALLY fast.
    static IntArray* __beginWrite() {
        IntArray* arr = (IntArray*)CurrentIsolate->NextOnHeap;
        new (arr) IntArray();
        //        *((ArrayHeader*)arr) = ArrayHeader { Any, 8, 0 };
        CurrentIsolate->NextOnHeap += sizeof(IntArray);
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
    T* __values() {
        return (T*)(((uint8_t*)this) + sizeof(IntArray));
    }
    
    // Override of the List interface
    VALUE First() {
        return GetAt(0);
    }
    
    // Override of the List interface
    LIST Rest() {
        VectorRest* rest;
        if (Header.Count == 1) {
            return LIST_NIL();
        }
        rest = MALLOC_HEAP(VectorRest);
        new (rest) VectorRest(this,1);
        return LIST(rest);
    }
    
    // Override of the List interface
    int Count() {
        return Header.Count;
    }
    
    virtual bool IsCheapCount() {
        return true;
    }

    
    
    // Override of the List interface
    VALUE GetAt( int i ) {
        assert( i < Header.Count );
        return INTEGER(__values()[i]);
    }
    
    // Override of the List interface
    List* ReplaceAt( int i, VALUE v ) {
        assert( i < Header.Count );
        CheckType(v);
        if (RefCount < 2) {
            __values()[i] = v.Integer;
            return this;
        }
        
#ifdef USE_INTARRAY
        // Make a copy of the list
        CheckType(v);
        size_t size = sizeof(IntArray) + Header.Count * sizeof(T);
        IntArray* newList = (IntArray*)CurrentIsolate->NextOnHeap;
        memcpy( (void*)newList, (void*)this, size );
        CurrentIsolate->ReportHeapWrite(size);
        newList->__values()[i] = v.Integer;
        return newList;
#else
        // Upgrade to a bitmapped vector trie as it is faster for
        // deriving new lists.
        throw std::runtime_error("Not implemented yet");
#endif
    }
    
    void CheckType( VALUE v ) {
       if (!v.IsInteger()){
           throw std::runtime_error("Can only append integers to an integer array");
       }
    }
    
    // Override of the List interface
    List* Append( VALUE v ) {
#ifndef USE_VECTOR
        // Make a copy of the list
        CheckType(v);
        size_t size = sizeof(IntArray) + Header.Count * sizeof(T);
        IntArray* newList = (IntArray*)CurrentIsolate->NextOnHeap;
        memcpy( (void*)newList, (void*)this, size );
        __values()[newList->Header.Count] = v.Integer;
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
#ifndef USE_VECTOR
        if (!v.IsInteger()) {
            throw std::runtime_error("Can only add integers to integer array");
        }
        // Make a copy of the list
        size_t elemsize = Header.Count * sizeof(T);
        byte* newList = (byte*)CurrentIsolate->NextOnHeap;
        memcpy( newList, (void*)this, sizeof(IntArray) );
        memcpy( newList + sizeof(IntArray) + sizeof(T), (void*)this, elemsize );
        __values()[0] = v.Integer;
        CurrentIsolate->ReportHeapWrite(sizeof(IntArray) + elemsize + sizeof(T));
        ((IntArray*)newList)->Header.Count++;
        return (IntArray*)newList;
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
    
    List* Take( int i ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    List* Last( int i ) {
        throw std::runtime_error("Not implemented yet");
    }
    
    List* Skip( int i ) {
        throw std::runtime_error("Not implemented yet");
    }
    
};

}
}


#endif /* IntArray_hpp */

#endif
