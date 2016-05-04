//
//  Array.hpp
//  Addie
//
//  This code is optional. It will only be used if you compile with USE_OPTIMIZATIONS.
//  Skip this when you first want to learn about working with the Addie codebase.
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifdef USE_ARRAY

#ifndef Array_hpp
#define Array_hpp

#include "Optimized_IntArray.hpp"
#include "Optimized_Array.hpp"
#include "Value.hpp"
#include "Cons.hpp"


#include "Isolate.hpp"




                // For tracking down bugs, we can create a naive version of Addie.
                // If defined, we will always create fresh new arrays instead of using the
                // persistent vector optimizations.



// Arrays are allocated on the heap with the values directly attached after the count variable.
// They are the cheapest form of lists until you detect that new lists are derived from the list.
// Because of this, the reader will always create lists of this type if the size of the list
// is at most 16 elements. If the list is larger than that, a Vector will be created instead.
// Cons pairs are almost never used unless the program explicitly does Cons operations.
class Array : public List {
public:
    Array() {
    }

    int _count;
    
    // Let's just cooperate with the memory allocator to make things REALLY fast.
    static Array* __beginWrite() {
        Array* arr = (Array*)CurrentIsolate->NextOnHeap;
        new (arr) Array();
        CurrentIsolate->NextOnHeap += sizeof(Array);
        return (arr);
    }
    
    // It is only legal to call __write following __beginWrite given that
    // there are NO other allocations being done inbetween.
    void __write( VALUE v ) {
        (*((VALUE*)CurrentIsolate->NextOnHeap)) = v;
        CurrentIsolate->NextOnHeap += sizeof(VALUE);
        _count++;
    }
    
    void __endWrite() {
        CurrentIsolate->AlignNextHeap();
    }
    
    static Array* Create( VALUE v ) {
        auto arr = __beginWrite();
        arr->__write(v);
        arr->__endWrite();
        return arr;
    }
    
    // The actual elements are stored immediately after the header
    VALUE* __values() {
        return (VALUE*)(((uint8_t*)this) + sizeof(Array));
    }
    
    // Override of the List interface
    VALUE First() {
        return GetAt(0);
    }
    
    // Override of the List interface
    VALUE Rest() {
        VectorRest* rest;
        if (_count == 1) {
            return NIL();
        }
        rest = MALLOC_HEAP(VectorRest);
        new (rest) VectorRest(this,1);
        return LIST(QParenthesis,rest);
    }
    
    // Override of the List interface
    int Count() {
        return _count;
    }

    
    // Override of the List interface
    VALUE GetAt( int i ) {
        assert( i < _count );
        return __values()[i];
    }
    
    // Override of the List interface
    List* ReplaceAt( int i, VALUE v ) {
        assert( i < _count );
        if (RefCount == 0) {
            __values()[i] = v;
            return this;
        }
        
#ifndef USE_VECTOR
        // Make a copy of the list
        size_t size = sizeof(Array) + _count * sizeof(VALUE);
        Array* newList = (Array*)CurrentIsolate->NextOnHeap;
        memcpy( (void*)newList, (void*)this, size );
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
#ifndef USE_VECTOR
        // Make a copy of the list
        
        //int oldCount = _count;
        
        size_t size = sizeof(Array) + _count * sizeof(VALUE);
        Array* newList = (Array*)CurrentIsolate->NextOnHeap;
        memcpy( (void*)newList, (void*)this, size );
        newList->__values()[newList->_count] = v;
        newList->_count++;
        
        //int newCount = newList->Count();
        //assert( newCount == oldCount + 1 );
        
        CurrentIsolate->ReportHeapWrite(size + sizeof(VALUE));

        //std::cout << "Before:" << LIST(this).Print() << "\n";
        //std::cout << "Adding:" << v.Print() << "\n";
        //std::cout << "After:" << LIST(newList).Print() << "\n";
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
        // Make a copy of the list
        size_t elemsize = _count * sizeof(VALUE);
        byte* newList = (byte*)CurrentIsolate->NextOnHeap;
        memcpy( newList, (void*)this, sizeof(Array) );
        memcpy( newList + sizeof(Array) + sizeof(VALUE), (void*)this, elemsize );
        __values()[0] = v;
        CurrentIsolate->ReportHeapWrite(sizeof(Array) + elemsize + sizeof(VALUE));
        ((Array*)newList)->_count++;
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

#endif