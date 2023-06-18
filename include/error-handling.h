#ifndef LSPI_ERRORS_H
#define LSPI_ERRORS_H

#include <token.h>

#include <cstdio>
#include <cstdlib>

enum ErrorType {
  InvalidFile,
  FailedMemStream,
  TokenTooLong,
  InvalidToken,
  UnrecognizableCharacter,
  MissingDeclarator,
  NotFunction,
  TokenExpected,
  NullExpr,
  NullCond,
  IncompatibleType,
  NotSupported,
  DimShouldBePosInt,
  UnknownIdentifier,
  EmptyDeclarationStmt,
  RepeatedDeclaration
};

[[noreturn]] void ThrowError(Token *, ErrorType);
[[noreturn]] void ThrowMsgError(Token *, ErrorType, const char *);
inline void PrintLine(const char *begin_of_line) {
  const char *p = begin_of_line;
  while (*p != '\r' && *p != '\n' && *p != '\0' && *p != EOF) {
    putchar(*p);
    p++;
  }
  putchar('\n');
}

#endif