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
    if (IsClassicalParenthesis()) {
        startParen = '(';
        endParen = ')';
    } else {
        startParen = '[';
        endParen = ']';
    }
    if (Integer == 0) {
        res << startParen << endParen;
        return res.str();
    }
    if (self->GetCdr().IsCons()) {
        res << startParen;
        res << self->GetCar().Print();
        VALUE next = self->GetCdr();
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
        res << endParen;
    }
    else {
        res << startParen;
        res << self->GetCar().Print();
        if (!self->GetCdr().IsNil()) {
            res << " . ";
            res << self->GetCdr().Print();
        }
        res << endParen;
    }
    return res.str();
}