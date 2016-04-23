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

class UnexpectedEOF: public std::exception
{
};

// Single byte lookup table for character parsing
struct Char {
    unsigned int IsSymbol : 1;              // Any glyph that can be used as a lisp symbol
    unsigned int IsWhitespace : 1;          // space, tab, comma, cr, lf
    unsigned int ClojureSpecialMeaning : 1; // Standard Lisp and a few new ones {[#%: etc...
    unsigned int ParensAndQuotes : 1;       // Things that starts parser modes such as {[("`;
};

extern Char Chars[256]; // 256 byte quick lookup for UTF-8 bytes

class StreamReader {
    
public:
    StreamReader() {
    }
    
    virtual char Read() = 0;
    virtual void UnRead() = 0;
    
    
    static void SkipWhitespace(StreamReader* r) {
    start:
        char c = r->Read();
        
        if (Chars[c].IsWhitespace)
            goto start;
        else
            r->UnRead();
    }
};

typedef VALUE (*ParseSomething)( Isolate* isolate, StreamReader* r );
extern ParseSomething Parsers[128];


// Provides a stream of characters to the parser
class StringReader : public StreamReader {
public:
    std::string str;
    int pos = 0;
    
    StringReader( std::string s ) {
        str = s;
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
    
    static VALUE ParseForm( Isolate*  isolate, StreamReader* r ) {
        
        char c = r->Read();
        if ( Chars[c].ClojureSpecialMeaning ) {
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
               return fn( isolate, r );
           }
        }
        return isolate->Nil;
    }

   static VALUE ParseString( Isolate* isolate, StreamReader* r) {
        
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
        
        return STRING("error");
   }

   static VALUE ParseList( Isolate* isolate, StreamReader* r) {
       CONS list;
       CONS previous;
       
        while (true) {
            
            try {
                StreamReader::SkipWhitespace(r);
            }
            catch (UnexpectedEOF e) {
                // The original idea is to test parse errors from Clojure, but
                // they are not very nice. Besides, rolling our own will save time.
                throw std::runtime_error("Missing )");
            }
            char c = r->Read();
            if (c == ')') {
//                previous->GetCons()->cdr = NIL();
                return list;
            }

            r->UnRead(); // We are ready to consume any lisp form
            
            VALUE elem = ParseForm( isolate, r );
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
