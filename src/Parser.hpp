//
//  Parser.hpp
//  Starcounter Lisp
//
//  Created by Joachim Wester on 22/04/16.
//  Copyright © 2016 Starcounter AB.
//

#ifndef Parser_hpp
#define Parser_hpp


#include <iostream>
#include <string>  // for std::string
//#include <vector>  // for std::vector
//#include <numeric> // for std::accumulate
#include <sstream> // for std::ostringstream

#include "Cons.hpp"
#include "Vector.hpp"
#include "Isolate.hpp"
#include "Value.hpp"
#include "Heap.hpp"

class UnexpectedEOF: public std::exception
{
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



// Provides a stream of characters to the parser
class StringReader  {
public:
    char* str;
    int length = 0;
    int pos = 0;

    StringReader( const char* original, size_t len ) {
        char* pc = (char *)CurrentIsolate->Heap.SafeMalloc( len );
        memcpy( pc, original, len );
        str = pc;
        length = len;
        for( int i = 0 ; i < len ; i++ ) { // Let's support cool unicode!
            unsigned char c = pc[i];
            if (c > 127) {
               if (c == 226 ) { // First part of unicode ⏜ or ⏝ (9180,9181)
                   c = pc[++i];
                    if ( c== 143 ) { // Second part of unicode ⏜⏝ (9180,9181)
                       c = pc[++i];
                       if ( c== 156 ) { // Third part of unicode ⏜ (9180)
                           pc[i-2] = '(';
                           pc[i-1] = ' ';
                           pc[i] = ' ';
                       } else if ( c== 157 ) { // Third part of unicode ⏜ (9181)
                           pc[i-2] = ')';
                           pc[i-1] = ' ';
                           pc[i] = ' ';
                       }
                   }
               }
            }

            pc[i] = pc[i];
        }
    }
    
    StringReader( std::string str ) : StringReader( (char *)str.c_str(), str.length() ) {
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
    
    
    static void SkipWhitespace(StringReader* r) {
    start:
        unsigned char c = r->Read();
        
        if (Chars[c].IsWhitespace)
            goto start;
        else
            r->UnRead();
    }
};

typedef VALUE (*ParseSomething)( StringReader* r );
extern ParseSomething Parsers[128];



class Parser {
    public:
    
    static VALUE ParseUnsolicitedEndBracket( StringReader* r ) {
        throw std::runtime_error("] does not match any [");
        return NIL();
    }
    
    static VALUE ParseUnsolicitedEndParen( StringReader* r ) {
        throw std::runtime_error(") does not match any (");
        return NIL();
    }
    
    static VALUE ParseUnsolicitedEndCurly( StringReader* r ) {
        throw std::runtime_error("} does not match any {");
        return NIL();
    }
    
    static VALUE ParseSymbol( StringReader* r ) {
        
        r->UnRead();
        std::ostringstream res;
        
        while (true) {
            unsigned char c = r->ReadEofOk();
            if (Chars[c].IsSymbolTerminator) {
                r->UnRead();
                break;
            }
            res << c;
        }
        std::string str = res.str();
        return SYMBOL(str.c_str(), str.length());
    }
    
    
    static VALUE ParseNumber( StringReader* r ) {
        
        // TODO! Currently only supports integers
        // TODO! Also read decimal numbers and maybe other numeric literals. Check Clojure...
        r->UnRead();
        std::ostringstream res;
        
        while (true) {
            unsigned char c = r->ReadEofOk();
            if (!Chars[c].IsDigit) {
                r->UnRead();
                break;
            }
            res << c;
        }
        std::string str = res.str();
        return INTEGER( std::stoi(str) );
    }

    
    static VALUE ParseMinusOrSymbol( StringReader* r ) {
        throw std::runtime_error("Encountered a minus or symbol");
        return NIL();
    }

    
    
    static VALUE ParseVector( StringReader* r ) {
        throw std::runtime_error("Encountered a vector");
        return NIL();
    }
    
    static VALUE ParseComment( StringReader* r ) {
        throw std::runtime_error("Encountered a comment");
        return NIL();
    }
    
    static VALUE ParseMap( StringReader* r ) {
        throw std::runtime_error("Encountered a map");
        return NIL();
    }
    
    static VALUE ParseForm( StringReader* r ) {
        
        r->SkipWhitespace(r);
        unsigned char c = r->Read();
        if (c > 127 ) {
            std::cout << "Unicode!!" ;
            std::cout << (int)c ;
            return Parser::ParseSymbol( r );
        }
        else {
           ParseSomething fn = Parsers[c];
           if (fn == NULL) {
               std::ostringstream msg;
               msg << "Missing parser for character ";
               msg << (int)c;
               throw std::runtime_error(msg.str());
           }
           else {
//               std::cout << "Found parser for ";
//               std::cout << c;
//               std::cout << "\n";
               return fn( r );
           }
        }
      //  }
        return NIL();
    }

   static VALUE ParseString( StringReader* r) {
        
        std::ostringstream res;
        
        while (true) {
            try {
                char v = r->Read();
                switch (v) {
                    case '"':
                        // We've reached the end of the string literal
                        return STRING(res.str());
                    case '\\':
                        throw std::invalid_argument( "escape characters not implemented yet" );
                    default:
                        res << v;
                }
                
            }
            catch (UnexpectedEOF) {
                throw std::runtime_error("EOF while reading string");
            }
        }
        
        //return STRING("error");
   }

   static VALUE ParseList( StringReader* r) {
       CONS list;
       CONS previous;
       
        while (true) {
            
            try {
                StringReader::SkipWhitespace(r);
            }
            catch (UnexpectedEOF e) {
                throw std::runtime_error("Missing )");
            }
            unsigned char c = r->Read();
            if (c == ')' ) {
//                previous->GetCons()->cdr = NIL();
                return list;
            }

            r->UnRead(); // We are ready to consume any lisp form
            
            VALUE elem = ParseForm( r );
            if (previous.IsEmptyList()) {
                list = CONS( elem, NIL());
                previous = list;
            }
            else {
                previous = previous.SnocBANG( elem );
            }
        }
   }
};


#endif /* Parser_hpp */
