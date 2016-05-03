//
//  List.cpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
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
    VALUE next = self->Rest();
    if (Style == QString) {
        res << (char)(self->First().Integer);
        while (next.IsList()) {
            List* pnext = (List*)next.OtherBytes();
            res << (char)(pnext->First().Integer);
            next = pnext->Rest();
        }
    }
    else {
       res << self->First().Print();
       if (self->Rest().IsList()) {
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
           res << startParen;
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
    return ((List*)Integer)->ReplaceAt(i,v);
}

VALUE LIST::GetAt( int i ) {
    return ((List*)Integer)->GetAt(i);
}

