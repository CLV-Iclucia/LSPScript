//
// Created by creeper on 23-6-15.
//
#include <ast.h>
#include <error-handling.h>
#include <expressions.h>
#include <sparse-matrix.h>

#include <iostream>
#include <vector>

#define ASSIGN_SPM(ast, expr)                    \
  ast->type = Tp_Spm;                            \
  if (ast->eval.spm != nullptr) {                \
    spmx::SparseMatrixXd& dest = *(ast->eval.spm); \
    dest = (expr);                               \
  } else                                         \
    ast->eval.spm = new spmx::SparseMatrixXd(expr)

static AstNode *for_begin = nullptr, *for_cont = nullptr;
static std::vector<spmx::Triplet> t_list;
static uint loop_dep = 0;
void Exec(AstNode* cur);

static void CheckBinary(AstNode* cur) {
  if (cur->lhs == nullptr || cur->rhs == nullptr) ThrowError(cur->tk, NullExpr);
}

bool IsScalar(AstNode* cur) {
  return cur->type == Tp_Int || cur->type == Tp_Real;
}

bool IsInt(AstNode* cur) { return cur->type == Tp_Int; }

static bool IsReal(AstNode* cur) { return cur->type == Tp_Real; }

void CheckScalar(AstNode* cur) {
  if (!IsScalar(cur)) ThrowError(cur->tk, IncompatibleType);
}

void CheckInt(AstNode* cur) {
  if (!IsInt(cur)) ThrowError(cur->tk, IncompatibleType);
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
  if (cur->type == Tp_Int)
    return static_cast<Real>(cur->eval.int_num);
  else
    return cur->eval.real_num;
}

void CheckLvalue(AstNode* cur) {
  if (cur->sem != SM_Var) ThrowError(cur->tk, LvalueExpected);
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

static void Assign(Obj* obj, AstNode* rhs) {
  obj->type = rhs->type;
  if (rhs->type == Tp_Real) obj->eval.real_num = rhs->eval.real_num;
  if (rhs->type == Tp_Int) obj->eval.int_num = rhs->eval.int_num;
}

static void AssignInt(Obj* obj, int int_num) {
  obj->type = Tp_Int;
  obj->eval.int_num = int_num;
}

void GetTripletList(AstNode* cur) {
  if (cur->sem != SM_TripletList) ThrowError(cur->tk, CannotHappen);
  t_list.clear();
  t_list.reserve(cur->eval.int_num);
  cur = cur->nxt;
  while (cur) {
    if (cur->sem != SM_Triplet) ThrowError(cur->tk, CannotHappen);
    ExecIntBinary(cur);
    Exec(cur->ret);
    CheckScalar(cur->ret);
    t_list.emplace_back(cur->lhs->eval.int_num, cur->rhs->eval.int_num,
                        RealScalar(cur->ret));
    cur = cur->nxt;
  }
}

void ProcessAssign(AstNode* cur) {
  CheckBinary(cur);
  CheckLvalue(cur->lhs);
  if (cur->rhs->sem != SM_TripletList) Exec(cur->rhs);
  if (IsScalar(cur->rhs)) {
    if (cur->lhs->var->type == Tp_Spm) ThrowError(cur->tk, InvalidAssignment);
    Assign(cur->lhs->var, cur->rhs);
    if (IsReal(cur->rhs)) SetEvalReal(cur, cur->rhs->eval.real_num);
    if (IsInt(cur->rhs)) SetEvalInt(cur, cur->rhs->eval.int_num);
  }
  if (cur->rhs->sem == SM_TripletList || cur->rhs->type == Tp_Spm) {
    if (cur->lhs->var->type != Tp_Spm) ThrowError(cur->tk, InvalidAssignment);
    if (cur->rhs->sem == SM_TripletList) {
      GetTripletList(cur->rhs);
      cur->lhs->var->eval.spm->SetFromTriplets(t_list.begin(), t_list.end());
      cur->type = Tp_Spm;
    } else {
      const spmx::SparseMatrixXd& r_res = *(cur->rhs->eval.spm);
      if (cur->lhs->var->eval.spm != nullptr) {
        spmx::SparseMatrixXd& lvar = *(cur->lhs->var->eval.spm);
        lvar = r_res;
      } else
        cur->lhs->var->eval.spm =
            new spmx::SparseMatrixXd(r_res);
      cur->eval.spm = cur->lhs->var->eval.spm;
    }
  }
}

void ProcessMul(AstNode* cur) {
  ExecBinary(cur);
  if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
    if (IsInt(cur->lhs) && IsInt(cur->rhs))
      SetEvalInt(cur, cur->lhs->eval.int_num * cur->rhs->eval.int_num);
    else
      SetEvalReal(cur, RealScalar(cur->lhs) * RealScalar(cur->rhs));
  } else if (cur->rhs->type == Tp_Spm && IsScalar(cur->lhs)) {
    const spmx::SparseMatrixXd& rhs = *(cur->rhs->eval.spm);
    ASSIGN_SPM(cur, RealScalar(cur->lhs) * rhs);
  } else if (cur->lhs->type == Tp_Spm && IsScalar(cur->rhs)) {
    const spmx::SparseMatrixXd& rhs = *(cur->rhs->eval.spm);
    ASSIGN_SPM(cur, RealScalar(cur->rhs) * rhs);
  } else {
    const spmx::SparseMatrixXd& lhs = *(cur->lhs->eval.spm);
    const spmx::SparseMatrixXd& rhs = *(cur->rhs->eval.spm);
    ASSIGN_SPM(cur, lhs * rhs);
  }
}

void ProcessAdd(AstNode* cur) {
  ExecBinary(cur);
  if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
    if (IsInt(cur->lhs) && IsInt(cur->rhs))
      SetEvalInt(cur, cur->lhs->eval.int_num + cur->rhs->eval.int_num);
    else
      SetEvalReal(cur, RealScalar(cur->lhs) + RealScalar(cur->rhs));
  } else if (cur->lhs->type == Tp_Spm && cur->rhs->type == Tp_Spm) {
    const spmx::SparseMatrixXd& lhs = *(cur->lhs->eval.spm);
    const spmx::SparseMatrixXd& rhs = *(cur->rhs->eval.spm);
    ASSIGN_SPM(cur, lhs + rhs);
  } else
    ThrowError(cur->tk, InvalidOperationForScalarMat);
}

void ProcessSub(AstNode* cur) {
  ExecBinary(cur);
  if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
    if (IsInt(cur->lhs) && IsInt(cur->rhs))
      SetEvalInt(cur, cur->lhs->eval.int_num - cur->rhs->eval.int_num);
    else
      SetEvalReal(cur, RealScalar(cur->lhs) - RealScalar(cur->rhs));
  } else if (cur->lhs->type == Tp_Spm && cur->rhs->type == Tp_Spm) {
    const spmx::SparseMatrixXd& lhs = *(cur->lhs->eval.spm);
    const spmx::SparseMatrixXd& rhs = *(cur->rhs->eval.spm);
    ASSIGN_SPM(cur, lhs - rhs);
  } else
    ThrowError(cur->tk, InvalidOperationForScalarMat);
}

void ProcessDiv(AstNode* cur) {
  ExecBinary(cur);
  if (IsScalar(cur->lhs) && IsScalar(cur->rhs)) {
    if (IsInt(cur->lhs) && IsInt(cur->rhs))
      SetEvalInt(cur, cur->lhs->eval.int_num / cur->rhs->eval.int_num);
    else
      SetEvalReal(cur, RealScalar(cur->lhs) / RealScalar(cur->rhs));
  } else if (cur->rhs->type == Tp_Spm && IsScalar(cur->lhs)) {
    ThrowError(cur->tk, InvalidOperationForScalarMat);
  } else if (cur->lhs->type == Tp_Spm && IsScalar(cur->rhs)) {
    const spmx::SparseMatrixXd& lhs = *(cur->lhs->eval.spm);
    ASSIGN_SPM(cur, lhs * (1.0 / RealScalar(cur->rhs)));
  } else {
    ThrowError(cur->tk, InvalidOperationForMatrix);
  }
}

void FreeMatrix(AstNode* cur) {
  if (cur == nullptr) return;
  FreeMatrix(cur->lhs);
  FreeMatrix(cur->rhs);
  if ((cur->sem == SM_Add || cur->sem == SM_Sub || cur->sem == SM_Mul ||
       cur->sem == SM_Div) &&
      cur->type == Tp_Spm)
    delete cur->eval.spm;
}

void Exec(AstNode* cur) {
  while (true) {
    switch (cur->sem) {
      case SM_For:
        if (cur->init) Exec(cur->init);
        loop_dep++;
        for (;;) {
          if (cur->cond) {
            Exec(cur->cond);
            CheckInt(cur->cond);
            if (!cur->cond->eval.int_num) break;
          }
          if (cur->then) Exec(cur->then);
          if (cur->inc) Exec(cur->inc);
        }
        loop_dep--;
        break;
      case SM_Mul:
        ProcessMul(cur);
        break;
      case SM_Add:
        ProcessAdd(cur);
        break;
      case SM_Sub:
        ProcessSub(cur);
        break;
      case SM_Div:
        ProcessDiv(cur);
        break;
      case SM_Mod:
        ExecIntBinary(cur);
        SetEvalInt(cur, cur->lhs->eval.int_num % cur->rhs->eval.int_num);
        break;
      case SM_If:
        if (cur->cond == nullptr) ThrowError(cur->tk, NullCond);
        Exec(cur->cond);
        CheckInt(cur->cond);
        if (cur->cond->eval.int_num != 0)
          Exec(cur->then);
        else
          Exec(cur->else_then);
        break;
      case SM_Return:
        std::printf("Program returned.");
        if (cur->ret) {
          Exec(cur->ret);
          if (cur->ret->type == Tp_Undef)
            std::printf("The returned value is undefined.\n");
          if (IsInt(cur->ret))
            std::printf("The returned value is %d.\n", cur->ret->eval.int_num);
          if (IsReal(cur->ret))
            std::printf("The returned value is %lf.\n",
                        cur->ret->eval.real_num);
          if (cur->ret->type == Tp_Spm) {
            std::printf("Returned a sparse matrix of shape (%u, %u)\n",
                        cur->ret->eval.spm->Rows(), cur->ret->eval.spm->Cols());
            std::cout << *(cur->ret->eval.spm) << std::endl;
          }
        }
        exit(0);
      case SM_Comma:
        Exec(cur->lhs);
        Exec(cur->rhs);
        cur->type = cur->lhs->type;
        cur->eval = cur->lhs->eval;
        break;
      case SM_Assign:
        ProcessAssign(cur);
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
        else
          SetEvalInt(cur, RealScalar(cur->lhs) == RealScalar(cur->rhs));
        break;
      case SM_nEqual:
        ExecIntBinary(cur);
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num != cur->rhs->eval.int_num);
        else
          SetEvalInt(cur, RealScalar(cur->lhs) != RealScalar(cur->rhs));
        break;
      case SM_Lt:
        ExecScalarBinary(cur);
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num < cur->rhs->eval.int_num);
        else
          SetEvalInt(cur, RealScalar(cur->lhs) < RealScalar(cur->rhs));
        break;
      case SM_Le:
        ExecScalarBinary(cur);
        ExecScalarBinary(cur);
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num <= cur->rhs->eval.int_num);
        else
          SetEvalInt(cur, RealScalar(cur->lhs) <= RealScalar(cur->rhs));
        break;
      case SM_Gt:
        ExecScalarBinary(cur);
        ExecScalarBinary(cur);
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num > cur->rhs->eval.int_num);
        else
          SetEvalInt(cur, RealScalar(cur->lhs) > RealScalar(cur->rhs));
        break;
      case SM_Ge:
        ExecScalarBinary(cur);
        if (IsInt(cur->lhs) && IsInt(cur->rhs))
          SetEvalInt(cur, cur->lhs->eval.int_num >= cur->rhs->eval.int_num);
        else
          SetEvalInt(cur, RealScalar(cur->lhs) >= RealScalar(cur->rhs));
        break;
      case SM_LogNot:
        Exec(cur->lhs);
        CheckInt(cur->lhs);
        SetEvalInt(cur, !cur->lhs->eval.int_num);
        break;
      case SM_Neg:
        Exec(cur->lhs);
        if (IsInt(cur->lhs)) SetEvalInt(cur, -cur->lhs->eval.int_num);
        if (IsReal(cur->lhs)) SetEvalReal(cur, -cur->lhs->eval.real_num);
        break;
      case SM_Block:
        break;
      case SM_Inc:
        Exec(cur->lhs);
        CheckLvalue(cur->lhs);
        CheckInt(cur->lhs);
        SetEvalInt(cur, cur->lhs->var->eval.int_num);
        AssignInt(cur->lhs->var, cur->lhs->var->eval.int_num + 1);
        break;
      case SM_Dec:
        Exec(cur->lhs);
        CheckLvalue(cur->lhs);
        CheckInt(cur->lhs);
        SetEvalInt(cur, cur->lhs->var->eval.int_num);
        AssignInt(cur->lhs->var, cur->lhs->var->eval.int_num - 1);
        break;
      case SM_Const:
        return;
      case SM_Var:
        if (cur->var->type == Tp_Int)
          cur->eval.int_num = cur->var->eval.int_num;
        if (cur->var->type == Tp_Real)
          cur->eval.real_num = cur->var->eval.real_num;
        if (cur->var->type == Tp_Spm) cur->eval.spm = cur->var->eval.spm;
        cur->type = cur->var->type;
        return;
      case SM_TripletList:
        GetTripletList(cur);
      case SM_Triplet:
        ThrowError(cur->tk, CannotHappen);
    }
    if (cur->nxt) {
      if (loop_dep == 0) FreeMatrix(cur);
      cur = cur->nxt;
    } else
      return;
  }
}