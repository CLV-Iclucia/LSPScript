//
// Created by creeper on 23-5-28.
//

#include <ast.h>
#include <basic.h>
#include <error-handling.h>
#include <token.h>

static const uint kHashBucketSize = 1024;
static Token* cur = nullptr;
static Scope* cur_scope = nullptr;

static AstNode* ParseExpr();
static AstNode* ParseStmt();
static AstNode* ParseStmts();
static AstNode* ParseDeclaration();
static Obj* LookUpVar(Token* tk);
enum FindState { FoundAtCurrentScope, FoundAtOuterScope, NotFound };
static FindState FindVar(Token* tk);
static bool Consume(Token*& tk, const char* str) {
  if (Equal(tk, str)) {
    tk = tk->nxt;
    return true;
  }
  return false;
}

static void Skip(Token*& tk, const char* str) {
  if (Equal(tk, str))
    tk = tk->nxt;
  else
    ThrowMsgError(tk, TokenExpected, str);
}

/**
 * Following the method in chibicc, I use a stack (in fact implemented in linked
 * list) to manage the scopes
 */
static void EnterScope() {
  Scope* new_scope = new Scope;
  new_scope->nxt = cur_scope;
  cur_scope = new_scope;
}

static void LeaveScope() {
  Scope* prv_scope = cur_scope;
  cur_scope = cur_scope->nxt;
  delete prv_scope;
}

/**
 * currently I only support ident
 * declarator = ident
 * @return
 */
static Obj* ParseDeclarator() {
  if (cur->type != TK_Identifier) ThrowError(cur, MissingDeclarator);
  if (FindVar(cur) == FoundAtCurrentScope) ThrowError(cur, RepeatedDeclaration);
  Obj* obj = new Obj;
  obj->name = cur;
  cur = cur->nxt;
  return obj;
}

/**
 * primary = "("expr")" | ident | num
 * @return
 */
static AstNode* ParsePrimary() {
  AstNode* ast = nullptr;
  if (Consume(cur, "(")) {
    ast = ParseExpr();
    Skip(cur, ")");
    return ast;
  } else if (cur->type == TK_Identifier) {
    if (FindVar(cur) != NotFound) {
      Obj* obj = LookUpVar(cur);
      cur = cur->nxt;
      ast = new AstNode(SM_Var, obj);
      ast->tk = cur;
      return ast;
    } else
      ThrowError(cur, UnknownIdentifier);
  } else if (cur->type == TK_Int || cur->type == TK_Real) {
    ast = new AstNode(SM_Const, nullptr, nullptr);
    ast->tk = cur;
    if (cur->type == TK_Int) {
      ast->type = Tp_Int;
      ast->eval.int_num = cur->int_num;
    } else {
      ast->type = Tp_Real;
      ast->eval.real_num = cur->real_num;
    }
    cur = cur->nxt;
  }
  return ast;
}

/**
 * postfix = ident "(" func-args ")" postfix-tail* | primary postfix-tail*
 * postfix-tail = "["expr"]" | "("func-args")" | "++" | "--"
 */
static AstNode* ParsePostfix() {
  AstNode* ast = ParsePrimary();
  while (true) {
    if (Consume(cur, "[")) {
      ParseExpr();
      Skip(cur, "]");
    } else if (Consume(cur, "(")) {
      ThrowError(cur, NotSupported);
    } else if (Consume(cur, "++")) {
      ast = new AstNode(SM_Inc, ast, nullptr);
      ast->tk = cur;
    } else if (Consume(cur, "--")) {
      ast = new AstNode(SM_Dec, ast, nullptr);
      ast->tk = cur;
    } else
      break;
  }
  return ast;
}

/**
 * unary = "!"|"-" unary | postfix
 */
static AstNode* ParseUnary() {
  AstNode* ast = nullptr;
  if (Consume(cur, "!")) {
    ast = new AstNode(SM_LogNot, ParseUnary(), nullptr);
    ast->tk = cur;
    return ast;
  } else if (Consume(cur, "-")) {
    ast = new AstNode(SM_Neg, ParseUnary(), nullptr);
    ast->tk = cur;
    return ast;
  }
  return ParsePostfix();
}

/**
 * mul = unary ("*"|"/"|"%" unary)*
 */
static AstNode* ParseMul() {
  struct AstNode* ast = ParseUnary();
  ast->tk = cur;
  while (true) {
    if (Consume(cur, "*")) {
      ast = new AstNode(SM_Mul, ast, ParseUnary());
      ast->tk = cur;
    } else if (Consume(cur, "/")) {
      ast = new AstNode(SM_Div, ast, ParseUnary());
      ast->tk = cur;
    } else if (Consume(cur, "%")) {
      ast = new AstNode(SM_Mod, ast, ParseUnary());
      ast->tk = cur;
    } else
      break;
  }
  return ast;
}
/**
 * add = mul ("+"|"-" mul)*
 * @return
 */
static AstNode* ParseAdd() {
  AstNode* ast = ParseMul();
  while (true) {
    if (Consume(cur, "+")) {
      ast = new AstNode(SM_Add, ast, ParseMul());
      ast->tk = cur;
    }

    else if (Consume(cur, "-")) {
      ast = new AstNode(SM_Sub, ast, ParseMul());
      ast->tk = cur;
    } else
      break;
  }
  return ast;
}

/**
 * relation = add ("<"|">"|"<="|">=" add)*
 * @return
 */
static AstNode* ParseRelation() {
  struct AstNode* ast = ParseAdd();
  ast->tk = cur;
  while (true) {
    if (Consume(cur, "<")) {
      ast = new AstNode(SM_Lt, ast, ParseAdd());
      ast->tk = cur;
    } else if (Consume(cur, ">")) {
      ast = new AstNode(SM_Gt, ast, ParseAdd());
      ast->tk = cur;
    } else if (Consume(cur, "<=")) {
      ast = new AstNode(SM_Le, ast, ParseAdd());
      ast->tk = cur;
    } else if (Consume(cur, ">=")) {
      ast = new AstNode(SM_Ge, ast, ParseAdd());
      ast->tk = cur;
    } else
      break;
  }
  return ast;
}

/**
 * equality = relation ("=="|"!=" relation)*
 * @return
 */
static AstNode* ParseEquality() {
  struct AstNode* ast = ParseRelation();
  while (true) {
    if (Consume(cur, "==")) {
      ast = new AstNode(SM_Equal, ast, ParseRelation());
      ast->tk = cur;
    } else if (Consume(cur, "!=")) {
      ast = new AstNode(SM_nEqual, ast, ParseRelation());
      ast->tk = cur;
    } else
      break;
  }
  return ast;
}

/**
 * logand = equality ("&&" equality)*
 */
static AstNode* ParseLogAnd() {
  struct AstNode* ast = ParseEquality();
  while (Consume(cur, "&&")) {
    ast = new AstNode(SM_LogAnd, ast, ParseEquality());
    ast->tk = cur;
  }
  return ast;
}
/**
 * logor = logand (|| logand)*
 * @return
 */
static AstNode* ParseLogOr() {
  struct AstNode* ast = ParseLogAnd();
  while (Consume(cur, "||")) {
    ast = new AstNode(SM_LogOr, ast, ParseLogAnd());
    ast->tk = cur;
  }
  return ast;
}

/**
 * assign = logor (assign-op assign) ?
 */
static AstNode* ParseAssign() {
  struct AstNode* ast = ParseLogOr();
  if (Consume(cur, "=")) {
    ast = new AstNode(SM_Assign, ast, ParseAssign());
    ast->tk = cur;
  } else if (Consume(cur, "*=")) {
    ast = new AstNode(SM_Assign, ast, new AstNode(SM_Mul, ast, ParseAssign()));
    ast->tk = cur;
  } else if (Consume(cur, "/=")) {
    ast = new AstNode(SM_Assign, ast, new AstNode(SM_Div, ast, ParseAssign()));
    ast->tk = cur;
  } else if (Consume(cur, "+=")) {
    ast = new AstNode(SM_Assign, ast, new AstNode(SM_Add, ast, ParseAssign()));
    ast->tk = cur;
  } else if (Consume(cur, "-=")) {
    ast = new AstNode(SM_Assign, ast, new AstNode(SM_Sub, ast, ParseAssign()));
    ast->tk = cur;
  }
  return ast;
}

/**
 * expr = assign ("," expr)?
 * @return
 */
static AstNode* ParseExpr() {
  struct AstNode* ast = ParseAssign();
  if (Consume(cur, ",")) {
    ast = new AstNode(SM_Comma, ast, ParseExpr());
    ast->tk = cur;
  }
  return ast;
}

static AstNode* ParseExprStmt() {
  if (Equal(cur, ";")) return nullptr;
  AstNode* ast = ParseExpr();
  Skip(cur, ";");
  return ast;
}

/**
 * simplified BNF for stmt
 * stmt = "return" expr? ";"
 * @return
 */
static AstNode* ParseStmt() {
  if (Consume(cur, ";")) return nullptr;
  AstNode* ast = nullptr;
  if (Consume(cur, "return")) {
    ast = new AstNode(SM_Return);
    ast->sem = SM_Return;
    ast->ret = ParseExpr();
    Skip(cur, ";");
    return ast;
  } else if (Consume(cur, "if")) {
    Skip(cur, "(");
    ast = new AstNode(SM_If);
    ast->cond = ParseExpr();
    Skip(cur, ")");
    ast->then = ParseStmt();
    if (Consume(cur, "else")) ast->else_then = ParseStmt();
    return ast;
  } else if (Consume(cur, "for")) {
    Skip(cur, "(");
    EnterScope();
    ast = new AstNode(SM_For);
    if (Equal(cur, "var")) {
      ast->init = ParseDeclaration();
    } else if (Equal(cur, "vec") || Equal(cur, "spm"))
      ThrowError(cur, ComplexTypeInitNotAllowed);
    else
      ast->init = ParseExprStmt();
    ast->cond = ParseExpr();
    Skip(cur, ";");
    ast->inc = ParseExpr();
    Skip(cur, ")");
    ast->then = ParseStmt();
    LeaveScope();
    return ast;
  } else if (Consume(cur, "{"))
    return ParseStmts();
  else
    return ParseExprStmt();
}

static AstNode* ParseStmts() {
  EnterScope();
  AstNode* ast = new AstNode(SM_Block);
  AstNode* tail = ast;
  while (!Consume(cur, "}")) {
    if (Equal(cur, "var") || Equal(cur, "spm") || Equal(cur, "vec")) {
      AstNode* decl = ParseDeclaration();
      if (decl == nullptr) continue;
      tail->nxt = decl;
      tail = decl;
      continue;
    }
    tail->nxt = ParseStmt();
    tail = tail->nxt;
  }
  LeaveScope();
  return ast;
}

/**
 * triplet = "{" assign "," assign "," assign "}"
 */
static AstNode* ParseTriplet() {
  AstNode* ast = new AstNode(SM_Triplet);
  Skip(cur, "{");
  ast->lhs = ParseAssign();
  Skip(cur, ",");
  ast->rhs = ParseAssign();
  Skip(cur, ",");
  ast->ret = ParseAssign();
  Skip(cur, "}");
  return ast;
}

/**
 * triplet-list = "{" triplet* "}"
 * @return
 */
static AstNode* ParseTripletList() {
  AstNode* ast = new AstNode(SM_TripletList);
  ast->tk = cur;
  Skip(cur, "{");
  AstNode* tail = ast;
  uint cnt = 0;
  while (!Equal(cur, "}")) {
    if (cnt) Skip(cur, ",");
    tail->nxt = ParseTriplet();
    tail = tail->nxt;
    cnt++;
  }
  // I use this to save the number of triplets in this list.
  ast->eval.int_num = static_cast<int>(cnt);
  Skip(cur, "}");
  return ast;
}

static void AddScope(Obj* obj) {
  if (cur_scope->vars == nullptr) cur_scope->vars = new HashMap<64, Obj*>;
  cur_scope->vars->insert(obj->name->loc, obj->name->len, obj);
}
/**
 * declare-spm = "spm""<" uint "," uint ">" declarator ("=" triplet-list)?";"
 * declare-vec = "vec""<" uint ">" declarator ("=" scalar-list)?";"
 */
static AstNode* ParseMatVecDeclaration() {
  bool is_spm = false, is_vec = false;
  if (Consume(cur, "spm"))
    is_spm = true;
  else if (Consume(cur, "vec"))
    is_vec = true;
  uint m = 0, n = 0;
  if (is_spm || is_vec) {
    Skip(cur, "<");
    if (cur->type != TK_Int || cur->int_num <= 0)
      ThrowError(cur, DimShouldBePosInt);
    m = cur->int_num;
    cur = cur->nxt;
    if (is_vec) Skip(cur, ">");
    if (is_spm) {
      Skip(cur, ",");
      if (cur->type != TK_Int || cur->int_num <= 0)
        ThrowError(cur, DimShouldBePosInt);
      n = cur->int_num;
      cur = cur->nxt;
      Skip(cur, ">");
    }
  }

  Obj* obj = ParseDeclarator();
  if (is_spm) {
    obj->type = Tp_Spm;
    obj->eval.spm = new spmx::SparseMatrixXd(m, n);
    AddScope(obj);
  } else
    ThrowError(cur, NotSupported);
  AstNode* ast = nullptr;
  if (Consume(cur, "=")) {
    if (Equal(cur, "{"))
      ast =
          new AstNode(SM_Assign, new AstNode(SM_Var, obj), ParseTripletList());
    else
      ast = new AstNode(SM_Assign, new AstNode(SM_Var, obj), ParseAssign());
    ast->tk = cur;
  }
  Skip(cur, ";");
  return ast;
}

static FindState FindVar(Token* tk) {
  ull hash_code = cur_scope->vars->hash(tk->loc, tk->len);
  for (Scope* scope = cur_scope; scope; scope = scope->nxt) {
    if (scope->vars == nullptr) continue;
    if (scope->vars->FindByHash(hash_code)) {
      if (scope == cur_scope)
        return FoundAtCurrentScope;
      else
        return FoundAtOuterScope;
    }
  }
  return NotFound;
}

static Obj* LookUpVar(Token* tk) {
  ull hash_code = cur_scope->vars->hash(tk->loc, tk->len);
  for (Scope* scope = cur_scope; scope; scope = scope->nxt) {
    if (scope->vars == nullptr) continue;
    if (scope->vars->FindByHash(hash_code))
      return scope->vars->QueryByHash(hash_code);
  }
  return nullptr;
}

/**
 * declaration = declare-var | declare-spm | declare-vec
 * declare-var = "var"(declarator ("=" assign)?("," declarator("="
 * assign)?)*)?";"
 */
static AstNode* ParseDeclaration() {
  if (Equal(cur, "spm") || Equal(cur, "vec")) return ParseMatVecDeclaration();
  Skip(cur, "var");
  uint decl_cnt = 0;
  AstNode* ast = nullptr;
  while (!Equal(cur, ";")) {
    if (decl_cnt > 0) Skip(cur, ",");
    decl_cnt++;
    Obj* obj = ParseDeclarator();
    AddScope(obj);
    if (Consume(cur, "=")) {
      AstNode* decl = ParseAssign();
      decl = new AstNode(SM_Assign, new AstNode(SM_Var, obj), decl);
      if (decl_cnt == 1)
        ast = decl;
      else {
        ast = new AstNode(SM_Comma, ast, decl);
        ast->tk = cur;
      }
    }
  }
  Skip(cur, ";");
  if (decl_cnt == 0) ThrowError(cur, EmptyDeclarationStmt);
  return ast;
}

/**
 * prog = (declaration | stmt) *
 * @return
 */
static AstNode* ParseProg() {
  EnterScope();
  AstNode* ast = new AstNode(SM_Block);
  ast->tk = cur;
  AstNode* tail = ast;
  while (cur->type != TK_Eof) {
    if (Equal(cur, "var") || Equal(cur, "spm") || Equal(cur, "vec")) {
      AstNode* decl = ParseDeclaration();
      if (decl == nullptr) continue;
      tail->nxt = decl;
      tail = decl;
      continue;
    }
    AstNode* stmt = ParseStmt();
    tail->nxt = stmt;
    tail = stmt;
  }
  LeaveScope();
  return ast;
}

AstNode* Parse(Token* head) {
  cur = head;
  return ParseProg();
}