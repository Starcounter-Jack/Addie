//
//  Reader.hpp
//  Addie
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef Parser_hpp
#define Parser_hpp


#include <iostream>
#include <string>  // for std::string
//#include <vector>  // for std::vector
//#include <numeric> // for std::accumulate
#include <sstream> // for std::ostringstream

#include "VM.hpp"
#include "Cons.hpp"
#include "Optimized_Array.hpp"
#include "Optimized_IntArray.hpp"
#include "Optimized_Vector.hpp"

namespace Addie {
    namespace Internals {
    
class UnexpectedEOF: public std::exception {
};

// Single byte lookup table for character parsing
struct Char {
    unsigned int IsSymbolTerminator : 1;
    unsigned int IsSymbol : 1;              // Any glyph that can be used as a lisp symbol
    unsigned int IsWhitespace : 1;          // space, tab, comma, cr, lf
    unsigned int IsClojureSpecialMeaning : 1; // Standard Lisp and a few new ones {[#%: etc...
    unsigned int IsDigit : 1;
};

extern Char Chars[256]; // 256 byte quick lookup for UTF-8 bytes

class StreamReader  {
public:
    virtual unsigned char ReadEofOk() = 0;
    virtual unsigned char Read() = 0;
    virtual void UnRead() = 0;
    
    
    static void SkipWhitespace(StreamReader* r) {
    start:
        unsigned char c = r->ReadEofOk();
        
        if (Chars[c].IsWhitespace)
            goto start;
        else
            r->UnRead();
    }

};

// Provides a stream of characters to the parser
class StringReader : public StreamReader {
public:
    const char* str;
    int length = 0;
    int pos = 0;

    StringReader( const char* original, size_t len ) {
        //char* pc = (char *)CurrentIsolate->Heap.SafeMalloc( len ); // TODO! GC
        //memcpy( pc, original, len );
        str = original;
        length = len;
    }
    
    unsigned char ReadEofOk() {
        if (pos >= length) {
            pos++;
            return 0;
        }
        char c = str[pos];
        pos++;
        return c;
    }
    
    unsigned char Read() {
        if (pos >= length) {
            throw UnexpectedEOF();
        }
        unsigned char c = str[pos];
        pos += 1;
        return c;
    }
    
    void UnRead() {
        pos--;
    }
};

typedef VALUE (*ParseSomething)( StreamReader* r );
extern ParseSomething Parsers[128];



class Parser {
    public:
    
    static VALUE ParseForm( StreamReader* r );
    static VALUE ParseString( StreamReader* r);
    static VALUE ParseList( StreamReader* r);
    static VALUE ParseSymbol( StreamReader* r );
    static VALUE ParseNumber( StreamReader* r );
    static VALUE ParseVector( StreamReader* r);
   
    static VALUE ParseMinusOrSymbol( StreamReader* r ) {
        throw std::runtime_error("Encountered a minus or symbol");
        return NIL();
    }
    
    static VALUE ParseComment( StreamReader* r ) {
        throw std::runtime_error("Encountered a comment");
        return NIL();
    }
    
    static VALUE ParseCurly( StreamReader* r );
    
    static VALUE ParseUnsolicitedEndBracket( StreamReader* r ) {
        throw std::runtime_error("] does not match any [");
        return NIL();
    }
    
    static VALUE ParseUnsolicitedEndParen( StreamReader* r ) {
        throw std::runtime_error(") does not match any (");
        return NIL();
    }
    
    static VALUE ParseUnsolicitedEndCurly( StreamReader* r ) {
        throw std::runtime_error("} does not match any {");
        return NIL();
    }
    
    static bool ConsumeCharacter( StreamReader* r, bool eatIt, unsigned char part1 ) {
        unsigned char c = r->ReadEofOk();
        if (c == part1 ) {
            if (!eatIt) {
                r->UnRead();
            }
            return true;
        }
        r->UnRead();
        return false;
    }
    
    static bool ConsumeUnicodeCharacter( StreamReader* r, bool eatIt, unsigned char part1, unsigned char part2, unsigned char part3 )  {
        unsigned char c;
        c = r->ReadEofOk();
        if (c == part1 ) { // First part of unicode such as for example ⏜ or ⏝ (9180,9181)
            c = r->ReadEofOk();
            if ( c== part2 ) { // Second part of unicode
                c = r->ReadEofOk();
                if ( c== part3 ) { // Third part of unicode
                    if (!eatIt) {
                        r->UnRead();
                        r->UnRead();
                        r->UnRead();
                    }
                    return true;
                }
                r->UnRead();
            }
            r->UnRead();
        }
        r->UnRead();
        return false;
    }
    
    static bool ConsumeStartBracket( StreamReader* r, bool eatit ) {
        return ConsumeCharacter(r, eatit, '[') ||  ConsumeUnicodeCharacter(r, eatit, 226, 142, 180) || ConsumeUnicodeCharacter(r, eatit, 239, 185, 135);  // unicode ⎴ 9140
    }

    
    static bool ConsumeStartParenthesis( StreamReader* r, bool eatIt ) {
        return ConsumeCharacter(r, eatIt, '(') || ConsumeUnicodeCharacter(r, eatIt, 226, 143, 156); // unicode ⏜ (9180)
    }


    static bool ConsumeStartCurly( StreamReader* r, bool eatIt ) {
        return ConsumeCharacter(r, eatIt, '{') || ConsumeUnicodeCharacter(r, eatIt, 226, 143, 158)
        || ConsumeUnicodeCharacter(r,eatIt, 239,184,183) ; // unicode
    }

    
    // We support some exotic unicode end parenthesis
    static bool CheckForEndParenthesis( StreamReader* r ) {
        return ConsumeCharacter(r, true, ')') || ConsumeUnicodeCharacter(r,true,226,143,157); // unicode ⏝ (9181)
    }
    
    
    // We support some exotic unicode end parenthesis
    static bool CheckForEndCurly( StreamReader* r ) {
        return ConsumeCharacter(r,true,'}') || ConsumeUnicodeCharacter(r, true, 226, 143 ,159 ) || ConsumeUnicodeCharacter(r, true, 239, 184 ,184 );
    }
    
    
    // We support some exotic unicode end parenthesis
    static bool CheckForEndBracket( StreamReader* r ) {
        return ConsumeCharacter(r, true, ']') || ConsumeUnicodeCharacter(r, true, 226, 142 ,181 ) || ConsumeUnicodeCharacter(r,true,239,185,136); // unicode ⎵ (9141)
    }
};

}

}

#endif /* Parser_hpp */
