//
// Created by creeper on 23-6-5.
//
#include <ast.h>

#include <iostream>

void VisualizeTokenStream() {

}

static void DfsAst(AstNode* cur) {
  switch (cur->sem) {
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
    case SM_For:
      break;
    case SM_Return:
      break;
  }
}

void VisualizeAst(AstNode* cur, const char* output_path) {
  FILE* fp = fopen(output_path, "w+");
  if (!fp) {
    std::cerr << "Error: fail to open or create output file" << std::endl;
    exit(-1);
  }
  fprintf(fp, "digraph Ast {");
  DfsAst(cur);
  fprintf(fp, "}");
  fclose(fp);
}