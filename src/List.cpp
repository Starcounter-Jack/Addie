//
//  List.cpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "List.hpp"
#include "Cons.hpp"
#include <sstream>
#include "Optimized_Array.hpp"

#ifdef USE_CONS
LIST LIST::Append( VALUE elem ) {
    if (Integer == 0) {
        LIST v = LIST(elem,NIL());
        v.Style = this->Style;
        return v;
    }
    return LIST(Style,GetList()->Append( elem ));
}
#else
#ifdef USE_ARRAY
LIST LIST::Append( VALUE elem ) {
    if (Integer == 0) {
        return LIST(Style,Array::Create( elem ));
    }
    return LIST(Style,GetList()->Append(elem));
}
#endif
#endif

#ifdef USE_CONS
List* List::Prepend( VALUE v ) {
        LIST newList(v,LIST(QParenthesis,this));
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



