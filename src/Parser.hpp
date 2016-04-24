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

class StreamReader {
    
public:
    StreamReader() {
    }
    
    virtual char Read() = 0;
    virtual char ReadEofOk() = 0;
    virtual void UnRead() = 0;
    
    
    static void SkipWhitespace(StreamReader* r) {
    start:
        unsigned char c = r->Read();
        
        if (Chars[c].IsWhitespace)
            goto start;
        else
            r->UnRead();
    }
};

typedef VALUE (*ParseSomething)( StreamReader* r );
extern ParseSomething Parsers[128];


// Provides a stream of characters to the parser
class StringReader : public StreamReader {
public:
    std::string str;
    int pos = 0;
    
    StringReader( std::string s ) {
        str = s;
    }
    
    char ReadEofOk() {
        if (pos >= str.length()) {
            return 0;
        }
        char c = str[pos];
        pos += 1;
        return c;
    }
    
    char Read() {
        if (pos >= str.length()) {
            throw UnexpectedEOF();
        }
        char c = str[pos];
        pos += 1;
        return c;
    }
    
    void UnRead() {
        pos -= 1;
    }
};




class Parser {
    public:
    
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
    
    static VALUE ParseSymbol( StreamReader* r ) {
        
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
    
    
    static VALUE ParseNumber( StreamReader* r ) {
        
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

    
    static VALUE ParseMinusOrSymbol( StreamReader* r ) {
        throw std::runtime_error("Encountered a minus or symbol");
        return NIL();
    }

    
    
    static VALUE ParseVector( StreamReader* r ) {
        throw std::runtime_error("Encountered a vector");
        return NIL();
    }
    
    static VALUE ParseComment( StreamReader* r ) {
        throw std::runtime_error("Encountered a comment");
        return NIL();
    }
    
    static VALUE ParseMap( StreamReader* r ) {
        throw std::runtime_error("Encountered a map");
        return NIL();
    }
    
    static VALUE ParseForm( StreamReader* r ) {
        
        unsigned char c = r->Read();
       // if ( Chars[c].ClojureSpecialMeaning ) {
           ParseSomething fn = Parsers[c];
           if (fn == NULL) {
               std::string msg = "Missing parser for character ";
               msg += c;
               throw std::runtime_error(msg);
           }
           else {
//               std::cout << "Found parser for ";
//               std::cout << c;
//               std::cout << "\n";
               return fn( r );
           }
      //  }
        return NIL();
    }

   static VALUE ParseString( StreamReader* r) {
        
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

   static VALUE ParseList( StreamReader* r) {
       CONS list;
       CONS previous;
       
        while (true) {
            
            try {
                StreamReader::SkipWhitespace(r);
            }
            catch (UnexpectedEOF e) {
                throw std::runtime_error("Missing )");
            }
            char c = r->Read();
            if (c == ')') {
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
