//
//  Value.cpp
//  Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Value.hpp"
#include "Cons.hpp"
//#include "Isolate.hpp"


std::string STRING::ToString() {
    if (Integer == 0) {
        return "\"\"";
    }
    auto str =  (char*) Integer;
    str += sizeof(String);
    return str;
}


uint8_t* VALUE::OtherBytes() {
    return (uint8_t*)(Integer);
}

uint8_t* STRING::StringBytes() {
    return (uint8_t*)(Integer+sizeof(String));
}


void STRING::Print() {
    std::cout << "\"";
    std::cout << ToString();
    std::cout << "\"";
}

void STRING::AllocateString( const char* original, size_t size ) {
    
    size_t totalSize = size + sizeof(String) + 1;
    String* obj = (String*)CurrentIsolate->MallocHeap(totalSize+1); // We add a zero termination so we have a
    // cheap conversion
    // to zero terminated strings if needed. TODO! GC
    char* str = (char*)obj;
    str += sizeof(String);

    memcpy(str, original, size);
    str[size] = 0;
    this->Integer = (uint64_t)obj;
    
    new (obj) String(size);

}

uint32_t STRING::Length() {
    return ((String*)StringBytes())->Length;
}




SYMBOL::SYMBOL( const char* str, size_t len ) {
    Type = PSymbol;
    Integer = CurrentIsolate->RegisterSymbol( str, len, -1 );
}

std::string SYMBOL::ToString() {
    return CurrentIsolate->GetStringFromSymbolId(Integer); // vectors are zero based. symbols start with 1
}

void SYMBOL::Print() {
    std::cout << ToString();
    // std::cout << "#";       // Uncomment these to see symbol integer
    // std::cout << Integer;   // Uncomment these to see symbol integer
}


void NIL::Print() {
    std::cout << "nil";
}


void VALUE::Print() {
        switch (Type) {
            case (TString) :
                ((STRING*)this)->Print();
                return;
            case (TCons) :
                ((CONS*)this)->Print();
                return;
            case (PSymbol) :
                ((SYMBOL*)this)->Print();
                return;
            case (PNil) :
                ((NIL*)this)->Print();
                return;
            case (PInteger) :
                ((INTEGER*)this)->Print();
                return;
            default:
                break;
        }
    std::cout << "I dont know how to print a heap=";
    std::cout << " type=";
    std::cout << Type;
}

void INTEGER::Print() {
    std::cout << Integer;
}


