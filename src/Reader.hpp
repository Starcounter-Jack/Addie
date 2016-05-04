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

#include "Cons.hpp"
#include "Optimized_Vector.hpp"
#include "Isolate.hpp"
#include "Value.hpp"
#include "Optimized_Array.hpp"


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

class StreamReader  {
public:
    virtual unsigned char ReadEofOk() = 0;
    virtual unsigned char Read() = 0;
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
    
    
    static bool ConsumeVerticalStartBracket( StreamReader* r, bool eatit ) {
        unsigned char c;
        c = r->Read();
        if (c == '[') {
            if (!eatit) {
                r->UnRead();
            }
            return true;
        }
        if (c == 226 ) { // First part of unicode ⎴ 9140 or ⎵ 9141)
            c = r->Read();
            if ( c == 142 ) { // Second part
                c = r->Read();
                if ( c == 180 ) { // Third part
                    if (!eatit) {
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
        else if (c == 239 ) { // First part of unicode 65095 ﹇ 65096﹈)
            c = r->Read();
            if ( c == 185 ) { // Second part
                c = r->Read();
                if ( c == 135 ) { // Third part
                    if (!eatit) {
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

    
    static bool ConsumeVerticalStartParenthesis( StreamReader* r, bool eatIt ) {
        unsigned char c;
        c = r->Read();
        if (c == '(' ) {
            if (!eatIt) {
                r->UnRead();
            }
            return true;
        }
        if (c == 226 ) { // First part of unicode ⏜ or ⏝ (9180,9181)
            c = r->Read();
            if ( c== 143 ) { // Second part of unicode ⏜⏝ (9180,9181)
                c = r->Read();
                if ( c== 156 ) { // Third part of unicode ⏜ (9180)
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

    static VALUE ParseSymbol( StreamReader* r ) {
        
//        r->UnRead();
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
        if (str.length() == 0) {
            throw std::runtime_error("Symbol size is zero!");
        }
        return SYMBOL(str.c_str(), str.length());
    }
    
    
    static VALUE ParseNumber( StreamReader* r ) {
        
        // TODO! Currently only supports integers
        // TODO! Also read decimal numbers and maybe other numeric literals. Check Clojure...
        //r->UnRead();
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

    
    
    static VALUE ParseVector( StreamReader* r) {
        LIST list;
        list.Style = QBrackets;
        ConsumeVerticalStartBracket(r, true);
//        r->Read(); // Skip first parenthesis
        while (true) {
            
            try {
                StreamReader::SkipWhitespace(r);
            }
            catch (UnexpectedEOF e) {
                throw std::runtime_error("Missing ]");
            }
            
            if (CheckForEndBracket(r)) {
                return list;
            }
            
            VALUE elem = ParseForm( r );
            list = list.Append(elem);
            //list.Style = QBrackets;
        }
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
        
        r->SkipWhitespace(r);
        unsigned char c = r->Read();
        if (c > 127 ) {
            r->UnRead();
            if (ConsumeVerticalStartParenthesis(r,false)) {
                return Parser::ParseList(r);
            } else if (ConsumeVerticalStartBracket(r,false)) {
                return Parser::ParseVector(r);
            }
            return Parser::ParseSymbol(r);
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
               r->UnRead();
               return fn( r );
           }
        }
      //  }
        return NIL();
    }
    
    
    static VALUE ParseString( StreamReader* r) {
#ifdef USE_INTARRAY
        IntArray<char>* arr = IntArray<char>::__beginWrite();
        LIST list(QString,arr);
        try {
           r->Read(); // Skip the intitial "
           while (true) {
               unsigned char c = r->Read();
               if (c == '"' ) {
                   arr->__endWrite();
                   return list;
               }
               arr->__write(c);
           }
        }
        catch (...) {
            arr->__endWrite();
            throw;
        }
        arr->__endWrite();
        return list;
#else
        LIST list;
        r->Read(); // Skip the intitial "
        while (true) {
            unsigned char c = r->Read();
            if (c == '"' ) {
                list.Style = QString;
                return list;
            }
            VALUE elem = INTEGER(c);
            list = list.Append( elem );
        }
        return list;
#endif
    }

   static VALUE ParseStringOld( StreamReader* r) {
        
        std::ostringstream res;
       r->Read(); // Skip first "
        
        while (true) {
            try {
                char v = r->Read();
                switch (v) {
                    case '"':
                        // We've reached the end of the string literal
                        return STRINGOLD(res.str());
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
    
    // We support some exotic unicode end parenthesis
    static bool CheckForEndParenthesis( StreamReader* r ) {
        unsigned char c = r->Read();
        if (c == ')' ) {
            return true;
        }
        else if ( c == 226 ) { // Potential vertical end parenthesis
           // std::cout << "Spotted potential parens candidate";
            c = r->Read();
            if ( c== 143 ) { // Second part of unicode ⏝ (9181)
                c = r->Read();
                if ( c== 157 ) { // Third part of unicode ⏝ (9181)
                    return true; // Eureka! Vertical end paren
                }
                r->UnRead(); // This was not a vertical end paren
            }
            r->UnRead(); // This was not a vertical end paren
        }
        r->UnRead(); // This was not a vertical end paren
        return false;
    }
    
    
    // We support some exotic unicode end parenthesis
    static bool CheckForEndBracket( StreamReader* r ) {
        unsigned char c = r->Read();
        if (c == ']' ) {
            return true;
        }
        else if ( c == 226 ) { // Potential vertical end parenthesis
            c = r->Read();
            if ( c== 142 ) { // Second part of unicode ⎵(9141)
                c = r->Read();
                if ( c== 181 ) { // Third part of unicode ⎵ (9141)
                    return true; // Eureka! Vertical end paren
                }
                r->UnRead(); // This was not a vertical end paren
            }
            r->UnRead(); // This was not a vertical end paren
        }
        else if ( c == 239 ) { // Potential vertical end parenthesis
            c = r->Read();
            if ( c== 185 ) { // Second part of unicode ⎵(9141)
                c = r->Read();
                if ( c== 136 ) { // Third part of unicode ⎵ (9141)
                    return true; // Eureka! Vertical end paren
                }
                r->UnRead(); // This was not a vertical end paren
            }
            r->UnRead(); // This was not a vertical end paren
        }

        r->UnRead(); // This was not a vertical end paren
        return false;
    }
    




   static VALUE ParseList( StreamReader* r) {
       LIST list;
       
       ConsumeVerticalStartParenthesis(r, true);
        while (true) {
            
            try {
                StreamReader::SkipWhitespace(r);
            }
            catch (UnexpectedEOF e) {
                throw std::runtime_error("Missing )");
            }

            if (CheckForEndParenthesis(r)) {
                return list;
            }
            
            VALUE elem = ParseForm( r );
            list = list.Append( elem );
        }
   }
};


#endif /* Parser_hpp */
