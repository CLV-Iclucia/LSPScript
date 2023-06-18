//
// Created by creeper on 23-6-15.
//
#include <ast.h>
#include <error-handling.h>
static AstNode *for_begin = nullptr, *for_cont = nullptr;

void Exec(AstNode* cur);

static void CheckBinary(AstNode* cur) {
  if (cur->lhs == nullptr || cur->rhs == nullptr)
    ThrowError(cur->tk, NullExpr);
}

bool IsScalar(AstNode* cur) {
  return cur->type == Tp_Int || cur->type == Tp_Real;
}

bool IsInt(AstNode* cur) {
  return cur->type == Tp_Int;
}

static bool IsReal(AstNode* cur) {
  return cur->type == Tp_Real;
}

void CheckScalar(AstNode* cur) {
  if (!IsScalar(cur))
    ThrowError(cur->tk, IncompatibleType);
}

void CheckInt(AstNode* cur) {
  if (!IsInt(cur))
    ThrowError(cur->tk, IncompatibleType);
}

void SetEvalInt(AstNode* cur, int eval) {
  cur->type = Tp_Int;
  cur->eval.int_num = eval;
}

void SetEvalReal(AstNode* cur, Real eval) {
  cur->type = Tp_Int;
  cur->eval.real_num = eval;
}

Real RealScalar(AstNode* cur) {
  if (cur->type == Tp_Int) return static_cast<Real>(cur->eval.int_num);
  else return cur->eval.real_num;
}

static void ExecScalarBinary(AstNode* cur) {
  CheckBinary(cur);
  Exec(cur->lhs);
  CheckScalar(cur->lhs);
  Exec(cur->rhs);
  CheckScalar(cur->rhs);
}

static void ExecBinary(AstNode* cur) {
  CheckBinary(cur);
  Exec(cur->lhs);
  Exec(cur->rhs);
}

static void ExecIntBinary(AstNode* cur) {
  CheckBinary(cur);
  Exec(cur->lhs);
  CheckInt(cur->lhs);
  Exec(cur->rhs);
  CheckInt(cur->rhs);
}

void Exec(AstNode* cur) {
  switch (cur->sem) {
    case SM_For:
      if (cur->init)
        cur = cur->init;
      else if (cur->cond) {
        cur = cur->cond;
        for_begin = cur->cond;
      }
      break;
    case SM_Mul:
      ExecBinary(cur);
      if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num * cur->rhs->eval.int_num);
        else SetEvalReal(cur, RealScalar(cur->lhs) * RealScalar(cur->rhs));
      }/* else if (IsVec(cur->lhs) || IsScalar(cur->rhs)) {
        SetEvalVec(cur, Vec(cur->lhs) * RealScalar(cur->rhs));
      } else if (IsSpm(cur->lhs) && IsScalar(cur)) {
        SetEvalSpm(cur, Spm(cur->lhs) * RealScalar(cur->rhs));
      }*/
      break;
    case SM_Add:
      ExecBinary(cur);
      if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num + cur->rhs->eval.int_num);
        else SetEvalReal(cur, RealScalar(cur->lhs) + RealScalar(cur->rhs));
      }/* else if (IsVec(cur->lhs) && IsScalar(cur)) {
        SetEvalVec(cur, Vec(cur->lhs) + RealScalar(cur->rhs));
      } else if (IsSpm(cur->lhs) && IsScalar(cur)) {
        SetEvalSpm(cur, Spm(cur->lhs) + RealScalar(cur->rhs));
      }*/
      break;
    case SM_Sub:
      ExecBinary(cur);
      if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num - cur->rhs->eval.int_num);
        else SetEvalReal(cur, RealScalar(cur->lhs) - RealScalar(cur->rhs));
      }/* else if (IsVec(cur->lhs) && IsScalar(cur)) {
        SetEvalVec(cur, Vec(cur->lhs) - RealScalar(cur->rhs));
      } else if (IsSpm(cur->lhs) && IsScalar(cur)) {
        SetEvalSpm(cur, Spm(cur->lhs) - RealScalar(cur->rhs));
      }*/
      break;
    case SM_Div:
      ExecBinary(cur);
      if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num / cur->rhs->eval.int_num);
        else SetEvalReal(cur, RealScalar(cur->lhs) / RealScalar(cur->rhs));
      }/* else if (IsVec(cur->lhs) && IsScalar(cur)) {
        SetEvalVec(cur, Vec(cur->lhs) / RealScalar(cur->rhs));
      } else if (IsSpm(cur->lhs) && IsScalar(cur)) {
        SetEvalSpm(cur, Spm(cur->lhs) / RealScalar(cur->rhs));
      }*/
      break;
    case SM_Mod:
      ExecIntBinary(cur);
      SetEvalInt(cur, cur->lhs->eval.int_num % cur->rhs->eval.int_num);
      break;
    case SM_If:
      if (cur->cond == nullptr)
        ThrowError(cur->tk, NullCond);
      Exec(cur->cond);
      CheckInt(cur->cond);
      if (cur->cond->eval.int_num != 0)
        Exec(cur->then);
      else Exec(cur->else_then);
      break;
    case SM_Return:
      break;
    case SM_Comma:
      Exec(cur->lhs);
      Exec(cur->rhs);
      break;
    case SM_Assign:
      break;
    case SM_LogOr:
      ExecIntBinary(cur);
      SetEvalInt(cur, cur->lhs->eval.int_num || cur->rhs->eval.int_num);
      break;
    case SM_LogAnd:
      ExecIntBinary(cur);
      SetEvalInt(cur, cur->lhs->eval.int_num && cur->rhs->eval.int_num);
      break;
    case SM_Equal:
      ExecScalarBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num == cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) == RealScalar(cur->rhs));
      break;
    case SM_nEqual:
      ExecIntBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num != cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) != RealScalar(cur->rhs));
      break;
    case SM_Lt:
      ExecScalarBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num < cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) < RealScalar(cur->rhs));
      break;
    case SM_Le:
      ExecScalarBinary(cur);
      ExecScalarBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num <= cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) <= RealScalar(cur->rhs));
      break;
    case SM_Gt:
      ExecScalarBinary(cur);
      ExecScalarBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num > cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) > RealScalar(cur->rhs));
      break;
    case SM_Ge:
      ExecScalarBinary(cur);
      if (IsInt(cur->lhs) && IsInt(cur->rhs))
        SetEvalInt(cur, cur->lhs->eval.int_num >= cur->rhs->eval.int_num);
      else SetEvalInt(cur, RealScalar(cur->lhs) >= RealScalar(cur->rhs));
      break;
    case SM_LogNot:
      Exec(cur->lhs);
      CheckInt(cur->lhs);
      SetEvalInt(cur, !cur->lhs->eval.int_num);
      break;
    case SM_Neg:
      Exec(cur->lhs);
      if (IsInt(cur->lhs))
        SetEvalInt(cur, -cur->lhs->eval.int_num);
      if (IsReal(cur->lhs))
        SetEvalReal(cur, -cur->lhs->eval.real_num);
//      if (IsVec(cur->lhs))
  //      SetEvalVec(cur, -cur->lhs->eval.vec);
   //   if (IsSpm(cur->lhs))
    //    SetEvalVec(cur, -cur->lhs->eval.spm);
      break;
    case SM_Block:
      break;
    case SM_Inc:
      Exec(cur->lhs);
      CheckInt(cur->lhs);
      SetEvalInt(cur, cur->lhs->eval.int_num + 1);
      break;
    case SM_Dec:
      Exec(cur->lhs);
      CheckInt(cur->lhs);
      SetEvalInt(cur, cur->lhs->eval.int_num - 1);
      break;
    case SM_Const:
      return ;
  }
}