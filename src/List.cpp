//
//  List.cpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//
//

#include "List.hpp"
#include <sstream>



LIST LIST::Append( VALUE elem ) {
    return GetList()->Append( elem );
}

std::string LIST::Print() {
    std::ostringstream res;
    List* self = GetList();
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
    res << startParen;
    VALUE next = self->GetRest();
    if (Style == QString) {
        res << (char)(self->GetFirst().Integer);
        while (next.IsCons()) {
            List* pnext = (List*)next.OtherBytes();
            res << (char)(pnext->GetFirst().Integer);
            next = pnext->GetRest();
        }
    }
    else {
       res << self->GetFirst().Print();
       if (self->GetRest().IsCons()) {
           while (next.IsCons()) {
               res << " ";
               List* pnext = (List*)next.OtherBytes();
               res << pnext->GetFirst().Print();
               next = pnext->GetRest();
           }
           if (!next.IsNil()) {
               res << " . ";
               res << next.Print();
           }
       }
       else {
           res << startParen;
           res << self->GetFirst().Print();
           if (!self->GetRest().IsNil()) {
               res << " . ";
               res << self->GetRest().Print();
           }
       }
    }
    res << endParen;
    return res.str();
}



LIST LIST::SetAt( int i, VALUE v ) {
    return ((List*)Integer)->SetAt(i,v);
}

VALUE LIST::GetAt( int i ) {
    return ((List*)Integer)->GetAt(i);
}

