//
//  Value.cpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "VM.hpp"
#include "List.hpp"
#include "Cons.hpp"
#include <sstream>
#include "Optimized_Array.hpp"

using namespace Addie::Internals;


std::string STRINGOLD::ToString() {
    if (Integer == 0) {
        return "\"\"";
    }
    auto str =  (char*) Integer;
    str += sizeof(String);
    return str;
}


uint8_t* VALUE::OtherBytes() {
    return (uint8_t*)Integer;
}

uint8_t* STRINGOLD::StringBytes() {
    return (uint8_t*)(OtherBytes()+sizeof(String));
}


std::string STRINGOLD::Print() {
    std::ostringstream res;
    res << "\"";
    res << ToString();
    res << "\"";
    return res.str();
}

void STRINGOLD::AllocateString( const char* original, size_t size ) {
    
    size_t totalSize = size + sizeof(String) + 1;
    String* obj = (String*)CurrentIsolate->MallocHeap(totalSize+1); // We add a zero termination so we have a
    // cheap conversion
    // to zero terminated strings if needed. TODO! GC
    char* str = (char*)obj;
    str += sizeof(String);
    
    memcpy(str, original, size);
    str[size] = 0;
    Integer = (uint64_t)obj;
    
    new (obj) String(size);
    
}

uint32_t STRINGOLD::Length() {
    return ((String*)StringBytes())->Length;
}




SYMBOL::SYMBOL( const char* str, size_t len ) {
    Type = TAtom;
    AtomSubType = ASymbol;
    SymbolId = CurrentIsolate->RegisterSymbol( str, len, -1 );
}

std::string SYMBOL::ToString() {
    return CurrentIsolate->GetStringFromSymbolId(SymbolId); // vectors are zero based. symbols start with 1
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
            case (TList) :
                return this->PrintList();
            case (TOther) :
                return ((STRINGOLD*)this)->Print();
            case (TAtom) :
                switch (AtomSubType) {
                    case (ASymbol) :
                    case (AKeyword) :
                        return ((SYMBOL*)this)->Print();
                    case (ANil) :
                        return ((NIL*)this)->Print();
                    default:
                        break;
                }
            case (TNumber) :
            switch (NumberSubType) {
                case (NInteger) :
                    return ((INTEGER*)this)->Print();
                default:
                    break;
            }
    }

    std::ostringstream res;
    res << "I dont know how to print a heap=";
    res << " type=";
    res << Type;
    res << " subtype=";
    res << OtherSubType;
    return res.str();
}


std::string VALUE::ToString() {
    
    if (Type == TList && ListStyle == QString ) {
        return "hello";
    }
    return Print();
}

std::string INTEGER::Print() {
    std::ostringstream res;
    res << Integer;
    return res.str();
}



VALUE VALUE::Append( VALUE elem ) {
    if (ListPointer == 0) {
        VALUE v = VALUE(ListStyle,elem);
        return v;
    }
    return VALUE(ListStyle,GetList()->Append( elem ));
}


VALUE VALUE::Prepend( VALUE elem ) {
    if (ListPointer == 0) {
        VALUE v = VALUE(ListStyle,elem);
        return v;
    }
    return VALUE(ListStyle,GetList()->Prepend( elem ));
}



//    CONS newList;
//    newList.Rest = this;
//    newList.First = v;
//    return newList.GetList();
//}


VALUE VALUE::Rest() {
    if (Integer == 0) {
        return VALUE(QParenthesis);
    }
    /*
    List* self = GetList();
    if (self->IsCheapCount()) {
        if ( Start < self->Count() - 1 ) {
            LIST rest;
            rest.ListPointer = ListPointer;
            rest.Start = Start + 1;
            return rest;
        }
        return NIL();
    }
     */
    return GetList()->Rest();
}


VALUE VALUE::First() {
    assert( ListPointer != 0);
    return GetList()->First();
}



std::string VALUE::PrintList() {
    std::ostringstream res;
    char startParen;
    char endParen;
    
    switch (ListStyle) {
        case (QParenthesis):
            startParen = '(';
            endParen = ')';
            break;
        case (QBracket):
            startParen = '[';
            endParen = ']';
            break;
        case (QCurly):
            startParen = '{';
            endParen = '}';
            break;
        case (QString):
            startParen = '"';
            endParen = '"';
            break;
    }
    if (ListPointer == 0) {
        res << startParen << endParen;
        return res.str();
    }
    //List* self = GetList();
    
    res << startParen;
    VALUE next = Rest();
    if (ListStyle == QString) {
        res << (char)(First().Integer);
        while (!next.IsEmpty()) {
            //List* pnext = next.GetList();
            res << (char)(next.First().Integer);
            next = next.Rest();
        }
    }
    else {
       if (next.IsList()) {
           res << First().Print();
           while (!next.IsEmpty()) {
               res << " ";
               //List* pnext = next.GetList();
               res << next.First().Print();
               next = next.Rest();
           }
           if (!next.IsEmptyList()) {
               res << " . ";
               res << next.Print();
           }
       }
       else {
           //res << "Vafan";
           //res << startParen;
           res << First().Print();
           if (!Rest().IsEmptyList()) {
               res << " . ";
               res << Rest().Print();
           }
       }
    }
    res << endParen;
    return res.str();
}



VALUE VALUE::ReplaceAt( int i, VALUE v ) {
    return VALUE(ListStyle,((List*)Integer)->ReplaceAt(i,v));
}

VALUE VALUE::GetAt( int i ) {
    return GetList()->GetAt(i);
}

int VALUE::Count() {
        return GetList()->Count();
}


#ifdef USE_CONS
//List* LIST::CreateDefaultList( VALUE first, VALUE rest ) {
//    return Cons::Create(first,rest);
//}
#ifndef USE_ARRAY
List* VALUE::CreateDefaultList( VALUE first ) {
    return Cons::Create(first,VALUE(QParenthesis));
}
#endif
#endif

#ifdef USE_ARRAY
List* VALUE::CreateDefaultList( VALUE first ) {
    return Array::Create(first);
}
#endif



