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
    VALUE next = self->GetCdr();
    if (Style == QString) {
        res << (char)(self->GetCar().Integer);
        while (next.IsCons()) {
            List* pnext = (List*)next.OtherBytes();
            res << (char)(pnext->GetCar().Integer);
            next = pnext->GetCdr();
        }
    }
    else {
       res << self->GetCar().Print();
       if (self->GetCdr().IsCons()) {
           while (next.IsCons()) {
               res << " ";
               List* pnext = (List*)next.OtherBytes();
               res << pnext->GetCar().Print();
               next = pnext->GetCdr();
           }
           if (!next.IsNil()) {
               res << " . ";
               res << next.Print();
           }
       }
       else {
           res << startParen;
           res << self->GetCar().Print();
           if (!self->GetCdr().IsNil()) {
               res << " . ";
               res << self->GetCdr().Print();
           }
       }
    }
    res << endParen;
    return res.str();
}