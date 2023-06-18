//
// Created by creeper on 23-5-30.
//

#ifndef LSPI_BASIC_H
#define LSPI_BASIC_H

#include <ast.h>
#include <hash.h>
#include <token.h>

#include "sparse-matrix.h"
enum BasicType {
    Tp_Function,
    Tp_Int,
    Tp_Real,
    Tp_Undef,
    Tp_Triplet,
    Tp_Spm,
    Tp_Vector,
    Tp_Array
};

struct Obj {
  bool is_func;
  Token* name;
  Obj* params;
  union {
    int int_num;
    Real real_num;
    spmx::SparseMatrixXd *spm = nullptr;
  }eval;
  BasicType type = Tp_Undef;
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
