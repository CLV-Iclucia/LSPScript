//
// Created by creeper on 23-4-29.
//

#ifndef LSPI_LSP_TERMINAL_H
#define LSPI_LSP_TERMINAL_H

#include <cctype>
#include <cstdlib>
#include <cstring>

const uint kMaxTokenLength = 64;

enum TokenType {
  Undef,
  // Basic symbols
  Ast,     // *
  Lslash,  // /
  Perc,    // %
  And,     // &
  Vbar,    // |
  Plus,    // +
  Minus,   // -
  Eql,     // ==
  Sharp,   // #
  Lt,      // <
  Le,      // <=
  Gt,      // >
  Ge,      // >=
  Dot,     // .
  Comma,   // ,
  Colon,   // :
  Rrbra,   // )
  Rsbra,   // ]
  Rcbra,   // }
  Lrbra,   // (
  Lsbra,   // [
  Lcbra,   // {
  Tilde,   // ~
  Bang,    // !
  Assign,  // =
  Scolon,  // ;

  // Some reserved words
  If,     // if
  Else,   // else
  While,  // while
  For,    // for
  Const,  // const

  // Attributes
  TK_Identifier,
  TK_Number,
  TK_Punct,
  Module,
  TK_Eof
};




#endif  // LSPI_LSP_TERMINAL_H
