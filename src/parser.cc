//
// Created by creeper on 23-5-28.
//

#include <token.h>
#include <basic.h>
#include <error-handling.h>
#include <ast.h>

static const uint kHashBucketSize = 1024;
static Token *cur_tk = nullptr;
static Scope *cur_scope = nullptr;
static Scope *global = nullptr;
static Scope *local = nullptr;

static AstNode* ParseStmt();
static AstNode* ParseStmts();

/**
 * function_decl = "func" declarator type_suf
 * function = function_decl '{' compound_stmts
 * function_declaration = function_decl ';'
 * compound_stmts = (stmt)* '}'
 * function_decl = identifier ('('identifier_list')' | '(' ')')
 * identifier_list = (identifier',')* identifier
 * stmt =
 */

static void CreateParams(Type* tp) {

}
static bool Consume(Token*& tk, const char* str) {
  if(Equal(tk, str)) {
    tk = tk->nxt;
    return true;
  }
  return false;
}

static void Skip(Token*& tk, const char* str) {
  if(Equal(tk, str))
    tk = tk->nxt;
  else
    ThrowMsgError(tk, TokenExpected, str);
}

/**
 * Following the method in chibicc, I use a stack (in fact implemented in linked list) to manage the scopes
 */
static void EnterScope() {
  Scope *new_scope = new Scope;
  new_scope->nxt = cur_scope;
  cur_scope = new_scope;
}

static void LeaveScope() {
  Scope *prv_scope = cur_scope;
  cur_scope = cur_scope->nxt;
  delete prv_scope;
}

// funct-params = ident')'
static Type* ParseFunctParam(Token *tk) {
  if(Equal(tk, ")")) return nullptr;
  if(tk->type == TK_Identifier) {

  }
  Skip(tk, ")");
}

// type-suffix = ('(' funct_params) | ('(' array_dim) | epsilon
static Type* ParseTypeSuffix() {
  Type *tp = nullptr;
  if (Consume(cur_tk, "{")) {

  }
  if (Consume(cur_tk, "[")) {

  }

}

/**
 * declarator = ('(' ident ')' | '('declarator')' | ident) type-suf
 *
 * @return
 */
static Type* ParseDeclarator() {
  if (cur_tk->type != TK_Identifier)
    ThrowError(cur_tk, MissingDeclarator);
  cur_tk = cur_tk->nxt;
  Type *tp = new Type;
  tp = ParseTypeSuffix();
}

/**
 * expr = assign-op assign
 * @return
 */
static AstNode* ParseExpr() {

}

static AstNode* ParseExprStmt() {
  if (Equal(cur_tk, ";")) return nullptr;
  ParseExpr();
}

/**
 * simplified BNF for stmt
 * stmt = "return" expr? ";"
 * @return
 */
static AstNode* ParseStmt() {
  if (Consume(cur_tk, ";"))
    return nullptr;

  AstNode *ast = nullptr;
  if (Consume(cur_tk, "return")) {
    ast = new AstNode;
    ast->sem = SM_Return;
    ast->ret = ParseExpr();
    Skip(cur_tk, ";");
    return ast;
  }

  if (Consume(cur_tk, "if")) {
    cur_tk = cur_tk->nxt;
    Skip(cur_tk, "(");
    ast = new AstNode;
    ast->cond = ParseExpr();
    Skip(cur_tk, ")");
    ast->then = ParseStmt();
    if(Consume(cur_tk, "else")) {
      ast->else_then = ParseStmt();
    }
  }

  if (Consume(cur_tk, "for")) {
    Skip(cur_tk, "(");
    EnterScope();
    AstNode* nd = new AstNode;
    nd->init = ParseExprStmt();
    Skip(cur_tk, ";");
    nd->cond = ParseExpr();
    Skip(cur_tk, ";");
    nd->inc = ParseExpr();
    Skip(cur_tk, ")");
    nd->then = ParseStmt();
    LeaveScope();
  }

  if (Consume(cur_tk, "{")) {
    ParseStmts();
  }
}

static AstNode* ParseStmts() {
  EnterScope();
  while (!Equal(cur_tk, "}")) {
    ParseStmt();
  }
  LeaveScope();
}

static void ParseFunction() {
  Type *tp = ParseDeclarator();
  if (tp->type != Tp_Function)
    ThrowError(cur_tk, NotFunction);
  Skip(cur_tk, "{");
  CreateParams(tp);
  EnterScope();
  while (!Equal(cur_tk, "}")) {
    if (cur_tk->type == TK_Eof)
      ThrowMsgError(cur_tk, TokenExpected, "}");
    AstNode* nd = ParseExpr();
  }
  Skip(cur_tk, "}");
  LeaveScope();
}

/**
 * Triplet = "{"expr"," expr"," expr"}"
 */
static AstNode* ParseTriplets() {
  AstNode* ast = new AstNode;
  Skip(cur_tk, "{");
  ParseExpr();
  Skip(cur_tk, ",");
  ParseExpr();
  Skip(cur_tk, ",");
  ParseExpr();
  Skip(cur_tk, ",");
}

static AstNode* ParseProg() {
  EnterScope();
  if (Consume(cur_tk, "{")) {
    while(cur_tk->type != TK_Eof) {
      //    if (Consume(cur_tk, "func")) {
      //      // we parse it as a function definition
      //      ParseFunction();
      //    }
      ParseStmt();
    }
  } else
    ThrowMsgError(cur_tk, TokenExpected, "{");
  LeaveScope();
}

AstNode* Parse(Token *head) {
  cur_tk = head;
  cur_scope = global = new Scope();
  return ParseProg();
}