//
//  Value.cpp
//  Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Value.hpp"
#include "Cons.hpp"


std::string STRING::ToString() {
    if (Pointer == NULL) {
        return "\"\"";
    }
    return (char*) Pointer;
}

void STRING::Print() {
    std::cout << "\"";
    std::cout << ToString();
    std::cout << "\"";
}


void NIL::Print() {
    std::cout << "nil";
}



void VALUE::Print() {
    if (IsHeapObject == true) {
        switch (HType) {
            case (TString) :
                ((STRING*)this)->Print();
                return;
            case (TCons) :
                ((CONS*)this)->Print();
                return;
            default:
                break;
        }
    }
    else {
        switch (PType) {
            case (PNil) :
                ((NIL*)this)->Print();
                return;
            default:
                break;
        }

    }
    std::cout << "I dont know how to print a heap=";
    std::cout << IsHeapObject;
    std::cout << " htype=";
    std::cout << HType;
    std::cout << " ptype=";
    std::cout << PType;
}

