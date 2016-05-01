//
//  Starcounter Lisp
//
//  Parser.cpp
//  Parses Clojure syntax given UTF-8 input.
//  Needs unit tests badly.
//
//  Created by Joachim Wester on 21/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Parser.hpp"

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
    {1},          // 0   NULL
    {1},          // 1
    {1},          // 2
    {1},          // 3
    {1},          // 4
    {1},          // 5
    {1},          // 6
    {1},          // 7
    {1},          // 8
    {1,0,1},       // 9   TAB
    {1,0,1},       // 10  LF \n
    {1,0,1},       // 11  VERTICAL TAB
    {1,0,1},       // 12  FORM FEED
    {1,0,1},       // 13  CR \r
    {1},          // 14
    {1},          // 15
    {1},          // 16
    {1},          // 17
    {1},          // 18
    {1},          // 19
    {1},          // 20
    {1},          // 21
    {1},          // 22
    {1},          // 23
    {1},          // 24
    {1},          // 25
    {1},          // 26
    {1},          // 27
    {1},          // 28
    {1},          // 29
    {1},          // 30
    {1},          // 31
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
    {0,1},         // 128
    {0,1},         // 129
    {0,1},         // 130
    {0,1},         // 131
    {0,1},         // 132
    {0,1},         // 133
    {0,1},         // 134
    {0,1},         // 135
    {0,1},         // 136
    {0,1},         // 137
    {0,1},         // 138
    {0,1},         // 139
    {0,1},         // 140
    {0,1},         // 141
    {0,1},         // 142
    {0,1},         // 143
    {0,1},         // 144
    {0,1},         // 145
    {0,1},         // 146
    {0,1},         // 147
    {0,1},         // 148
    {0,1},         // 149
    {0,1},         // 150
    {0,1},         // 151
    {0,1},         // 152
    {0,1},         // 153
    {0,1},         // 154
    {0,1},         // 155
    {0,1},         // 156
    {0,1},         // 157
    {0,1},         // 158
    {0,1},         // 159
    {0,1},         // 160
    {0,1},         // 161
    {0,1},         // 162
    {0,1},         // 163
    {0,1},         // 164
    {0,1},         // 165
    {0,1},         // 166
    {0,1},         // 167
    {0,1},         // 168
    {0,1},         // 169
    {0,1},         // 170
    {0,1},         // 171
    {0,1},         // 172
    {0,1},         // 173
    {0,1},         // 174
    {0,1},         // 175
    {0,1},         // 176
    {0,1},         // 177
    {0,1},         // 178
    {0,1},         // 179
    {0,1},         // 180
    {0,1},         // 181
    {0,1},         // 182
    {0,1},         // 183
    {0,1},         // 184
    {0,1},         // 185
    {0,1},         // 186
    {0,1},         // 187
    {0,1},         // 188
    {0,1},         // 189
    {0,1},         // 190
    {0,1},         // 191
    {0,1},         // 192
    {0,1},         // 193
    {0,1},         // 194
    {0,1},         // 195
    {0,1},         // 196
    {0,1},         // 197
    {0,1},         // 198
    {0,1},         // 199
    {0,1},         // 200
    {0,1},         // 201
    {0,1},         // 202
    {0,1},         // 203
    {0,1},         // 204
    {0,1},         // 205
    {0,1},         // 206
    {0,1},         // 207
    {0,1},         // 208
    {0,1},         // 209
    {0,1},         // 210
    {0,1},         // 211
    {0,1},         // 212
    {0,1},         // 213
    {0,1},         // 214
    {0,1},         // 215
    {0,1},         // 216
    {0,1},         // 217
    {0,1},         // 218
    {0,1},         // 219
    {0,1},         // 220
    {0,1},         // 221
    {0,1},         // 222
    {0,1},         // 223
    {0,1},         // 224
    {0,1},         // 225
    {0,1},         // 226
    {0,1},         // 227
    {0,1},         // 228
    {0,1},         // 229
    {0,1},         // 230
    {0,1},         // 231
    {0,1},         // 232
    {0,1},         // 233
    {0,1},         // 234
    {0,1},         // 235
    {0,1},         // 236
    {0,1},         // 237
    {0,1},         // 238
    {0,1},         // 239
    {0,1},         // 240
    {0,1},         // 241
    {0,1},         // 242
    {0,1},         // 243
    {0,1},         // 244
    {0,1},         // 245
    {0,1},         // 246
    {0,1},         // 247
    {0,1},         // 248
    {0,1},         // 249
    {0,1},         // 250
    {0,1},         // 251
    {0,1},         // 252
    {0,1},         // 253
    {0,1},         // 254
    {0,1}          // 255
};







