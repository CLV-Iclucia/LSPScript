//
// Created by creeper on 23-5-28.
//
#include <token.h>

extern uint atoh(const char *);
extern uint atoo(const char *);

/*
TokenType Sym2Tk(const char *str, uint n) {
  TokenType tk = Undef;
  if (n == 1) {
    switch (str[0]) {
      case '*':
        tk = Ast;
        break;
      case '/':
        tk = Lslash;
        break;
      case '%':
        tk = Perc;
        break;
      case '&':
        tk = And;
        break;
      case '|':
        tk = Vbar;
        break;
      case '+':
        tk = Plus;
        break;
      case '-':
        tk = Minus;
        break;
      case '#':
        tk = Sharp;
        break;
      case '<':
        tk = Lt;
        break;
      case '>':
        tk = Gt;
        break;
      case '.':
        tk = Dot;
        break;
      case ',':
        tk = Comma;
        break;
      case ':':
        tk = Colon;
        break;
      case '(':
        tk = Lrbra;
        break;
      case '[':
        tk = Lsbra;
        break;
      case '{':
        tk = Lcbra;
        break;
      case ')':
        tk = Rrbra;
        break;
      case ']':
        tk = Rsbra;
        break;
      case '}':
        tk = Rcbra;
        break;
      case '~':
        tk = Tilde;
        break;
      case '!':
        tk = Bang;
        break;
      case '=':
        tk = Assign;
        break;
      case ';':
        tk = Scolon;
        break;
      default:
        ThrowError(InvalidToken);
    }
  } else if (n == 2) {
    if (strcmp(str, "<=") == 0)
      tk = Le;
    else if (strcmp(str, "==") == 0)
      tk = Eql;
    else if (strcmp(str, ">=") == 0)
      tk = Ge;
    else
      ThrowError(TokenTooLong);
  } else
    ThrowError(TokenTooLong);
  return tk;
}

TokenType AnalyzeToken(const char *str) {
  uint n = std::strlen(str);
  TokenType tk = Undef;
  if (isdigit(str[0])) {
    tk = TK_Number;
  } else if (!isalpha(str[0])) {
    tk = Sym2Tk(str, n);
  } else {
    // TODO: Variables and so on.
  }
  return tk;
}*/