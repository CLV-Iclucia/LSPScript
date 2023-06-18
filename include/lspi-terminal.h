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
  // Attributes
  TK_Identifier,
  TK_Int,
  TK_Real,
  TK_Punct,
  TK_Eof,
  TK_Reserved,
};




#endif  // LSPI_LSP_TERMINAL_H
