//
// Created by creeper on 23-5-19.
//

#ifndef LSPI_AST_H
#define LSPI_AST_H

#include <token.h>

#include <variant>

#include "basic.h"

enum Semantic {
  SM_Comma,
  SM_Mul,
  SM_Add,
  SM_Sub,
  SM_Div,
  SM_Mod,
  SM_If,
  SM_For,
  SM_Return,
  SM_Assign,
  SM_LogOr,
  SM_LogAnd,
  SM_Equal,
  SM_nEqual,
  SM_Lt,
  SM_Le,
  SM_Gt,
  SM_Ge,
  SM_LogNot,
  SM_Neg,
  SM_Block,
  SM_Inc,
  SM_Dec,
  SM_Const,
  SM_Var
};

struct AstNode {
  AstNode() = default;
  AstNode(Semantic sem_) : sem(sem_) {}
  AstNode(Semantic sem_, AstNode *lhs_, AstNode *rhs_)
      : sem(sem_), lhs(lhs_), rhs(rhs_) {}
  AstNode(Semantic sem_, Obj* obj) : sem(sem_), var(obj) {}
  Semantic sem;
  AstNode *lhs = nullptr, *rhs = nullptr;  // for expr
  AstNode *cond = nullptr, *then = nullptr,
          *else_then = nullptr;            // for if...else
  AstNode *init = nullptr, *inc = nullptr;
  AstNode *ret = nullptr;                  // for return
  AstNode *nxt = nullptr;
  union Eval{
    Real real_num;
    int int_num = 0;
//    SparseMatrixXd spm;
 //   VectorXd vec;
  }eval;
  BasicType type = Tp_Undef;

  Obj* var = nullptr;
  Token* tk = nullptr;
};

#endif  // LSPI_AST_H
