//
// Created by creeper on 23-5-30.
//

#ifndef LSPI_BASIC_H
#define LSPI_BASIC_H

#include <hash.h>
#include <token.h>

enum BasicType {
    Tp_Function,
    Tp_Int,
    Tp_Real,
    Tp_Triplet,
    Tp_Spm,
    Tp_Vector,
    Tp_Array
};

struct Type {
  BasicType type;
  Type* params = nullptr;
  Token* name = nullptr;
  Type* nxt = nullptr;
};

struct Scope {
  const char* name; // for debugging
  Scope *nxt = nullptr;
};

struct VarScope {
  Type* tp;
};


#endif  // LSPI_BASIC_H
