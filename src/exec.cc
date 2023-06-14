//
// Created by creeper on 23-6-15.
//
#include <ast.h>

static AstNode *for_begin = nullptr, *for_cont = nullptr;
static AstNode* cur = nullptr;

bool EvalCond(AstNode* nd) {
  if (nd->sem != SM_) {
  }
}

void exec() {
  while (true) {
    switch (cur->sem) {
      case SM_For:
        if (cur->init)
          cur = cur->init;
        else if (cur->cond) {
          cur = cur->cond;
          bool res = EvalCond(cur->cond);
          for_begin = cur->cond;
          if (res) {

          } else {

          }
        }
        break;
      case SM_Mul:
        break;
      case SM_Add:
        break;
      case SM_Sub:
        break;
      case SM_Div:
        break;
      case SM_If:
        break;
      case SM_Return:
        break;
    }
  }
}