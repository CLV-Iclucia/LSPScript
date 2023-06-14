//
// Created by creeper on 23-5-19.
//

#ifndef LSPI_AST_H
#define LSPI_AST_H

#include <token.h>

enum Semantic { SM_Mul, SM_Add, SM_Sub, SM_Div, SM_If, SM_For, SM_Return };

struct AstNode {
  Semantic sem;
  AstNode *lhs = nullptr, *rhs = nullptr; // for expr
  AstNode *cond = nullptr, *then = nullptr, *else_then = nullptr; // for if...else
  AstNode *init = nullptr, *inc = nullptr;
  AstNode *ret = nullptr; // for return
};

#endif  // LSPI_AST_H
