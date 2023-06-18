//
// Created by creeper on 23-6-5.
//
#include <ast.h>
#include <hash.h>

#include <iostream>
#include <queue>

void VisualizeTokenStream() {}
static BitTrie<bool, SIZEOF_PTR << 3> trie;
static std::queue<AstNode*> q;
static void OutputNode(FILE* fp, AstNode* cur) {
  std::fprintf(fp, "Node%zu[label=\"", reinterpret_cast<size_t>(cur));
  switch (cur->sem) {
    case SM_Mul:
      std::fprintf(fp, "Mul\"];");
      break;
    case SM_Add:
      std::fprintf(fp, "Add\"];");
      break;
    case SM_Sub:
      std::fprintf(fp, "Sub\"];");
      break;
    case SM_Div:
      std::fprintf(fp, "Div\"];");
      break;
    case SM_If:
      std::fprintf(fp, "If\"];");
      break;
    case SM_For:
      std::fprintf(fp, "For\"];");
      break;
    case SM_Return:
      std::fprintf(fp, "Return\"];");
      break;
    case SM_Comma:
      std::fprintf(fp, "Comma\"];");
      break;
    case SM_Mod:
      std::fprintf(fp, "Mod\"];");
      break;
    case SM_Assign:
      std::fprintf(fp, "Assign\"];");
      break;
    case SM_LogOr:
      std::fprintf(fp, "Log-Or\"];");
      break;
    case SM_LogAnd:
      std::fprintf(fp, "Log-And\"];");
      break;
    case SM_Equal:
      std::fprintf(fp, "Equal\"];");
      break;
    case SM_nEqual:
      std::fprintf(fp, "Not-Equal\"];");
      break;
    case SM_Lt:
      std::fprintf(fp, "Less-than\"];");
      break;
    case SM_Le:
      std::fprintf(fp, "Less-equal\"];");
      break;
    case SM_Gt:
      std::fprintf(fp, "Greater-than\"];");
      break;
    case SM_Ge:
      std::fprintf(fp, "Greater-equal\"];");
      break;
    case SM_LogNot:
      std::fprintf(fp, "Log-Not\"];");
      break;
    case SM_Neg:
      std::fprintf(fp, "Neg\"];");
      break;
    case SM_Block:
      std::fprintf(fp, "Block\"];");
      break;
    case SM_Inc:
      std::fprintf(fp, "Inc\"];");
      break;
    case SM_Dec:
      std::fprintf(fp, "Dec\"];");
      break;
    case SM_Const:
      std::fprintf(fp, "Const ");
      if (cur->type == Tp_Real) std::fprintf(fp, "%lf", cur->eval.real_num);
      if (cur->type == Tp_Int) std::fprintf(fp, "%d", cur->eval.int_num);
      std::fprintf(fp, "\"];");
      break;
    case SM_Var:
      std::fprintf(fp, "Var ");
      for (uint i = 0; i < cur->var->name->len; i++)
        fputc(cur->var->name->loc[i], fp);
      std::fprintf(fp, "\"];");
      break;
    case SM_TripletList:
      std::fprintf(fp, "Triplet List\"];");
      break;
    case SM_Triplet:
      std::fprintf(fp, "Triplet\"];");
      break;
  }
  std::fprintf(fp, "\n");
}

static void OutputEdgeAndPush(FILE* fp, AstNode* cur, AstNode* ch,
                              const char* label) {
  if (ch != nullptr) {
    if (!trie.lookup(ch)) {
      OutputNode(fp, ch);
      q.push(ch);
      trie.insert(ch);
    }
    std::fprintf(fp, "Node%zu->Node%zu[label=\"%s\"];\n",
                 reinterpret_cast<size_t>(cur), reinterpret_cast<size_t>(ch),
                 label);
  }
}

static void OutputAst(FILE* fp, AstNode* ast) {
  q.push(ast);
  trie.insert(ast);
  OutputNode(fp, ast);
  while (!q.empty()) {
    AstNode* cur = q.front();
    q.pop();
    OutputEdgeAndPush(fp, cur, cur->lhs, "lhs");
    OutputEdgeAndPush(fp, cur, cur->rhs, "rhs");
    OutputEdgeAndPush(fp, cur, cur->nxt, "nxt");
    OutputEdgeAndPush(fp, cur, cur->init, "init");
    OutputEdgeAndPush(fp, cur, cur->cond, "cond");
    OutputEdgeAndPush(fp, cur, cur->then, "then");
    OutputEdgeAndPush(fp, cur, cur->else_then, "else");
    OutputEdgeAndPush(fp, cur, cur->inc, "inc");
    OutputEdgeAndPush(fp, cur, cur->ret, "ret");
  }
}

void VisualizeAst(AstNode* cur, const char* output_path,
                  const char* prog_name) {
  FILE* fp = fopen(output_path, "w+");
  if (!fp) {
    std::cerr << "Error: fail to open and create output file" << std::endl;
    exit(-1);
  }
  fprintf(fp, "digraph Ast {\n");
  OutputAst(fp, cur);
  fprintf(fp, "}");
  fclose(fp);
}