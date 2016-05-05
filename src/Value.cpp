//
//  Value.cpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "Addie.hpp"
#include "Cons.hpp"
#include <sstream>
#include "Optimized_Array.hpp"



std::string STRINGOLD::ToString() {
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

uint8_t* STRINGOLD::StringBytes() {
    return (uint8_t*)(Integer+sizeof(String));
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
    this->Integer = (uint64_t)obj;
    
    new (obj) String(size);
    
}

uint32_t STRINGOLD::Length() {
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
        case (PList) :
            return ((LIST*)this)->Print();
        case (PSymbol) :
            return ((SYMBOL*)this)->Print();
        case (PNil) :
            return ((NIL*)this)->Print();
        case (PNumber) :
            return ((INTEGER*)this)->Print();
        case (PStringOld) :
            return ((STRINGOLD*)this)->Print();
        default:
            break;
    }
    
    std::ostringstream res;
    res << "I dont know how to print a heap=";
    res << " type=";
    res << Type;
    return res.str();
}


std::string VALUE::ToString() {
    
    if (Type == PList && Style == QString ) {
        return "hello";
    }
    return Print();
}

std::string INTEGER::Print() {
    std::ostringstream res;
    res << Integer;
    return res.str();
}



#ifdef USE_ARRAY
LIST LIST::Append( VALUE elem ) {
    if (Integer == 0) {
        LIST v = LIST(elem);
        v.Style = this->Style;
        return v;
    }
    return LIST(Style,GetList()->Append( elem ));
}
#else
#ifdef USE_CONS
LIST LIST::Append( VALUE elem ) {
    if (Integer == 0) {
        return LIST(Style,elem);
    }
    return LIST(Style,GetList()->Append(elem));
}
#endif
#endif

#ifdef USE_CONS
List* List::Prepend( VALUE v ) {
        LIST newList(QParenthesis,v,LIST(QParenthesis,this));
        return newList.GetList();
}
#endif

//    CONS newList;
//    newList.Rest = this;
//    newList.First = v;
//    return newList.GetList();
//}


VALUE LIST::Rest() {
    if (Integer == 0) {
        return NIL();
    }
    return GetList()->Rest();
}


std::string LIST::Print() {
    std::ostringstream res;
    char startParen;
    char endParen;
    switch (Style) {
        case (QParenthesis):
            startParen = '(';
            endParen = ')';
            break;
        case (QBrackets):
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
    if (Integer == 0) {
        res << startParen << endParen;
        return res.str();
    }
    List* self = GetList();
    
    res << startParen;
    VALUE next = self->Rest();
    if (Style == QString) {
        res << (char)(self->First().Integer);
        while (next.IsList()) {
            List* pnext = next.GetList();
            res << (char)(pnext->First().Integer);
            next = pnext->Rest();
        }
    }
    else {
       if (self->Rest().IsList()) {
           res << self->First().Print();
           while (next.IsList()) {
               res << " ";
               List* pnext = (List*)next.OtherBytes();
               res << pnext->First().Print();
               next = pnext->Rest();
           }
           if (!next.IsNil()) {
               res << " . ";
               res << next.Print();
           }
       }
       else {
           //res << "Vafan";
           //res << startParen;
           res << self->First().Print();
           if (!self->Rest().IsNil()) {
               res << " . ";
               res << self->Rest().Print();
           }
       }
    }
    res << endParen;
    return res.str();
}



LIST LIST::ReplaceAt( int i, VALUE v ) {
    return LIST(Style,((List*)Integer)->ReplaceAt(i,v));
}

VALUE LIST::GetAt( int i ) {
    return ((List*)Integer)->GetAt(i);
}

#ifdef USE_CONS
void LIST::MaterializeAsCons( VALUE first, VALUE rest ) {
    auto c = Cons::Create(first,rest);
    //Cons* c = new Cons(first,rest);
    Integer = (uintptr_t)c;
    assert( c == (Cons*)Integer );
    c->CheckIntegrety();
    Cons* obj = (Cons*)Integer;
    assert( obj->MemoryCheck == 123456789 );
    CheckIntegrety();
}
#endif

#ifdef USE_ARRAY
void LIST::MaterializeAsArray( VALUE first ) {
    auto c = Array::Create(first);
    //Cons* c = new Cons(first,rest);
    Integer = (uintptr_t)c;
    assert( c == (Array*)Integer );
    c->CheckIntegrety();
    auto obj = (Array*)Integer;
    assert( obj->MemoryCheck == 123456789 );
    CheckIntegrety();
}
#endif



