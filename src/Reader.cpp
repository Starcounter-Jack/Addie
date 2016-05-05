//
//  Reader.cpp
//  Starcounter Lisp
//
//  Parses Clojure syntax given UTF-8 input.
//
//  Created by Joachim Wester on 21/04/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#include "Addie.hpp"
#include "Reader.hpp"

// All our reserved characters lives in ASCII space except for our extended
// begin/end parenthesis unicode symbols ⏜ ⏝ (9180 and 9181).
ParseSomething Parsers[128] = {
    NULL,    // 0
    NULL,    // 1
    NULL,    // 2
    NULL,    // 3
    NULL,    // 4
    NULL,    // 5
    NULL,    // 6
    NULL,    // 7
    NULL,    // 8
    NULL,    // 9
    NULL,    // 10
    NULL,    // 11
    NULL,    // 12
    NULL,    // 13
    NULL,    // 14
    NULL,    // 15
    NULL,    // 16
    NULL,    // 17
    NULL,    // 18
    NULL,    // 19
    NULL,    // 20
    NULL,    // 21
    NULL,    // 22
    NULL,    // 23
    NULL,    // 24
    NULL,    // 25
    NULL,    // 26
    NULL,    // 27
    NULL,    // 28
    NULL,    // 29
    NULL,    // 30
    NULL,    // 31
    NULL,    // 32
    &(Parser::ParseSymbol),    // 33
    &(Parser::ParseString),    // 34  "
    &(Parser::ParseSymbol),    // 35
    &(Parser::ParseSymbol),    // 36
    &(Parser::ParseSymbol),    // 37
    &(Parser::ParseSymbol),    // 38
    &(Parser::ParseSymbol),    // 39
    &(Parser::ParseList),    // 40 (
    &(Parser::ParseUnsolicitedEndParen),    // 41 )
    &(Parser::ParseSymbol),    // 42
    &(Parser::ParseSymbol),    // 43
    &(Parser::ParseSymbol),    // 44
    &(Parser::ParseMinusOrSymbol),    // 45 -
    &(Parser::ParseSymbol),    // 46 .
    &(Parser::ParseSymbol),    // 47 /
    &(Parser::ParseNumber),    // 48 0
    &(Parser::ParseNumber),    // 49 1
    &(Parser::ParseNumber),    // 50 2
    &(Parser::ParseNumber),    // 51 3
    &(Parser::ParseNumber),    // 52 4
    &(Parser::ParseNumber),    // 53 5
    &(Parser::ParseNumber),    // 54 6
    &(Parser::ParseNumber),    // 55 7
    &(Parser::ParseNumber),    // 56 8
    &(Parser::ParseNumber),    // 57 9
    NULL,    // 58 :
    &(Parser::ParseComment),   // 59 ;
    &(Parser::ParseSymbol),    // 60 <
    &(Parser::ParseSymbol),    // 61 =
    &(Parser::ParseSymbol),    // 62 >
    &(Parser::ParseSymbol),    // 63 ?
    NULL,     // 64 @
    &(Parser::ParseSymbol),    // 65 A
    &(Parser::ParseSymbol),    // 66 B
    &(Parser::ParseSymbol),    // 67 C
    &(Parser::ParseSymbol),    // 68 D
    &(Parser::ParseSymbol),    // 69 E
    &(Parser::ParseSymbol),    // 70 F
    &(Parser::ParseSymbol),    // 71 G
    &(Parser::ParseSymbol),    // 72 H
    &(Parser::ParseSymbol),    // 73 I
    &(Parser::ParseSymbol),    // 74 J
    &(Parser::ParseSymbol),    // 75 K
    &(Parser::ParseSymbol),    // 76 L
    &(Parser::ParseSymbol),    // 77 M
    &(Parser::ParseSymbol),    // 78 N
    &(Parser::ParseSymbol),    // 79 O
    &(Parser::ParseSymbol),    // 80 P
    &(Parser::ParseSymbol),    // 81 Q
    &(Parser::ParseSymbol),    // 82 R
    &(Parser::ParseSymbol),    // 83 S
    &(Parser::ParseSymbol),    // 84 T
    &(Parser::ParseSymbol),    // 85 U
    &(Parser::ParseSymbol),    // 86 V
    &(Parser::ParseSymbol),    // 87 W
    &(Parser::ParseSymbol),    // 88 X
    &(Parser::ParseSymbol),    // 89 Y
    &(Parser::ParseSymbol),    // 90 Z
    &(Parser::ParseVector),    // 91 [
    NULL,    // 92 BACKSLASH
    &(Parser::ParseUnsolicitedEndBracket),    // 93 ]
    NULL,    // 94 ^
    &(Parser::ParseSymbol),    // 95 _
    NULL,    // 96 `
    &(Parser::ParseSymbol),    // 97 a
    &(Parser::ParseSymbol),    // 98 b
    &(Parser::ParseSymbol),    // 99 c
    &(Parser::ParseSymbol),    // 100 d
    &(Parser::ParseSymbol),    // 101 e
    &(Parser::ParseSymbol),    // 102 f
    &(Parser::ParseSymbol),    // 103 g
    &(Parser::ParseSymbol),    // 104 h
    &(Parser::ParseSymbol),    // 105 i
    &(Parser::ParseSymbol),    // 106 j
    &(Parser::ParseSymbol),    // 107 k
    &(Parser::ParseSymbol),    // 108 l
    &(Parser::ParseSymbol),    // 109 m
    &(Parser::ParseSymbol),    // 110 n
    &(Parser::ParseSymbol),    // 111 o
    &(Parser::ParseSymbol),    // 112 p
    &(Parser::ParseSymbol),    // 113 q
    &(Parser::ParseSymbol),    // 114 r
    &(Parser::ParseSymbol),    // 115 s
    &(Parser::ParseSymbol),    // 116 t
    &(Parser::ParseSymbol),    // 117 u
    &(Parser::ParseSymbol),    // 118 v
    &(Parser::ParseSymbol),    // 119 w
    &(Parser::ParseSymbol),    // 120 x
    &(Parser::ParseSymbol),    // 121 y
    &(Parser::ParseSymbol),    // 122 z
    &(Parser::ParseMap),    // 123 {
    &(Parser::ParseSymbol),    // 124 |
    &(Parser::ParseUnsolicitedEndCurly),    // 125 }
    NULL,    // 126
    NULL     // 127
    //
};


Char Chars[256] = {
    {1},{1},{1},{1},{1},{1},{1},{1},{1}, // 0-8
    {1,0,1},       // 9   TAB
    {1,0,1},       // 10  LF \n
    {1,0,1},       // 11  VERTICAL TAB
    {1,0,1},       // 12  FORM FEED
    {1,0,1},       // 13  CR \r
    {1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},    // 14-31
    {1,0,1},       // 32 SPACE
    {0,1},         // 33  !
    {1,0,0,1},   // 34  "
    {0,0,0,1},   // 35  #
    {0,1},         // 36  $
    {0,0,0,1},   // 37  %
    {0,1},         // 38  &
    {0,0,0,1},   // 39  '
    {1,0,0,1},   // 40  (
    {1,0,0,1},   // 41  )
    {0,1},         // 42  *
    {0,1},         // 43  +
    {1,0},         // 44  ,
    {0,1},         // 45  -
    {0,1},         // 46  .
    {0,1},         // 47  /
    {0,1,0,0,1},         // 48  0
    {0,1,0,0,1},         // 49  1
    {0,1,0,0,1},         // 50  2
    {0,1,0,0,1},         // 51  3
    {0,1,0,0,1},         // 52  4
    {0,1,0,0,1},         // 53  5
    {0,1,0,0,1},         // 54  6
    {0,1,0,0,1},         // 55  7
    {0,1,0,0,1},         // 56  8
    {0,1,0,0,1},         // 57  9
    {1,0,0,1},   // 58  :
    {1,0,0,1},   // 59  ;
    {0,1},         // 60  <
    {0,1},         // 61  =
    {0,1},         // 62  >
    {0,1},         // 63  ?
    {1,0,0,1},   // 64  @
    {0,1},         // 65  A
    {0,1},         // 66  B
    {0,1},         // 67  C
    {0,1},         // 68  D
    {0,1},         // 69  E
    {0,1},         // 70  F
    {0,1},         // 71  G
    {0,1},         // 72  H
    {0,1},         // 73  I
    {0,1},         // 74  J
    {0,1},         // 75  K
    {0,1},         // 76  L
    {0,1},         // 77  M
    {0,1},         // 78  N
    {0,1},         // 79  O
    {0,1},         // 80  P
    {0,1},         // 81  Q
    {0,1},         // 82  R
    {0,1},         // 83  S
    {0,1},         // 84  T
    {0,1},         // 85  U
    {0,1},         // 86  V
    {0,1},         // 87  W
    {0,1},         // 88  X
    {0,1},         // 89  Y
    {0,1},         // 90  Z
    {1,0,0,1},   // 91  [
    {1,0,0,1},   // 92  BACKSLASH
    {1,0,0,1},   // 93  ]
    {1,0,0,1},   // 94  ^
    {0,1},         // 95  _
    {1,0,0,1},   // 96  `
    {0,1},         // 97  a
    {0,1},         // 98  b
    {0,1},         // 99  c
    {0,1},         // 100 d
    {0,1},         // 101 e
    {0,1},         // 102 f
    {0,1},         // 103 g
    {0,1},         // 104 h
    {0,1},         // 105 i
    {0,1},         // 106 j
    {0,1},         // 107 k
    {0,1},         // 108 l
    {0,1},         // 109 m
    {0,1},         // 110 n
    {0,1},         // 111 o
    {0,1},         // 112 p
    {0,1},         // 113 q
    {0,1},         // 114 r
    {0,1},         // 115 s
    {0,1},         // 116 t
    {0,1},         // 117 u
    {0,1},         // 118 v
    {0,1},         // 119 w
    {0,1},         // 120 x
    {0,1},         // 121 y
    {0,1},         // 122 z
    {1,0,0,1},   // 123 {
    {0,1},         // 124 |
    {1,0,0,1},   // 125 }
    {1,0,0,1},   // 126 ~
    {1},          // 127 DELETE
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
    {0,1},{0,1}
};







