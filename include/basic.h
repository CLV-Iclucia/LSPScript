//
// Created by creeper on 23-5-30.
//

#ifndef LSPI_BASIC_H
#define LSPI_BASIC_H

#include <ast.h>
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
  Type* nxt = nullptr;
  uint rows = 0, cols = 0;
};

struct Obj {
  bool is_func;
  Token* name;
  Type* tp;
  Obj* params;
};

struct Scope {
  const char* name; // for debugging
  HashMap<64, Obj*>* vars = nullptr;
  Scope *nxt = nullptr;
  ~Scope() {
    delete vars;
  }
};

#endif  // LSPI_BASIC_H
