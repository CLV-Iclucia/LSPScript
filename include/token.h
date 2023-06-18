//
// Created by creeper on 23-5-28.
//

#ifndef LSPI_TOKEN_H
#define LSPI_TOKEN_H

#include <lspi-terminal.h>
#include <lspi-types.h>

#include <cstdio>

struct Token {
  uint line_no = 0;
  char *loc = nullptr;  // points to the string
  uint len = 0;
  TokenType type = Undef;
  Token *nxt = nullptr;
  uint offset;  ///< offset relative to the first character of this line, used
                ///< for error report
  int int_num;
  Real real_num;
  const char *error_msg;
};

inline bool Equal(Token *tk, const char *str) {
  if (memcmp(tk->loc, str, tk->len) == 0 && str[tk->len] == '\0') return true;
  return false;
}

inline void PrintTokenString(Token *tk) {
  for (int i = 0; i < tk->len; i++) putchar(tk->loc[i]);
}

inline void PrintToken(Token *tk) {
  switch (tk->type) {
    case TK_Identifier:
      std::printf("Identifier ");
      PrintTokenString(tk);
      putchar('\n');
      break;
    case TK_Real:
      std::printf("Real ");
      PrintTokenString(tk);
      putchar('\n');
      break;
    case TK_Int:
      std::printf("Int ");
      PrintTokenString(tk);
      putchar('\n');
      break;
    case TK_Eof:
      std::printf("End of file\n");
      break;
    case TK_Punct:
      std::printf("Punctuation ");
      PrintTokenString(tk);
      putchar('\n');
      break;
    case TK_Reserved:
      std::printf("Reserved ");
      PrintTokenString(tk);
      putchar('\n');
      break;
    default:
      std::printf("Unknown token\n");
  }
}
#endif  // LSPI_TOKEN_H
