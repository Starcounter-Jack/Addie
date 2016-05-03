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
#include <sstream>

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


std::string STRING::Print() {
        std::ostringstream res;
    res << "\"";
    res << ToString();
    res << "\"";
    return res.str();
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

std::string SYMBOL::Print() {
    std::ostringstream res;
    res << ToString();
    // std::cout << "#";       // Uncomment these to see symbol integer
    // std::cout << Integer;   // Uncomment these to see symbol integer
    return res.str();
}


std::string NIL::Print() {
    std::ostringstream res;
    res << "nil";
    return res.str();
}


std::string VALUE::Print() {
        switch (Type) {
            case (TString) :
                return ((STRING*)this)->Print();
            case (PList) :
                return ((LIST*)this)->Print();
            case (PSymbol) :
                return ((SYMBOL*)this)->Print();
            case (PNil) :
                return ((NIL*)this)->Print();
            case (PInteger) :
                return ((INTEGER*)this)->Print();
            default:
                break;
        }
    
        std::ostringstream res;
    res << "I dont know how to print a heap=";
    res << " type=";
    res << Type;
    return res.str();
}

std::string INTEGER::Print() {
        std::ostringstream res;
    res << Integer;
    return res.str();
}


