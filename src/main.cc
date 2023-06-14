#include <cstdio>
#include <cstring>
#include <ast.h>

extern size_t ReadCode(const char*);
extern Token* Tokenize();
extern AstNode* Parse(Token *head);
extern void VisualizeAst(AstNode* ast, const char* output_path, const char* prog_name);
extern void PrintAst(AstNode *ast, const char *path);
int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("Lspi command line launched.\n");
  } else if ((std::strcmp(argv[1], "-h") == 0) ||
             (std::strcmp(argv[1], "--help") == 0)) {
    std::printf("Usage: lspi [option] ... [file] [arg] ...\n");
  } else {
    std::printf("Reading code from %s...\n", argv[1]);
    size_t tot = ReadCode(argv[1]);
    std::printf("Reading code done. Totally %zu bytes are successfully read.\n", tot);
    std::printf("---------------------------------------------------------------------------\n");
    std::printf("Start tokenizing...\n");
    Token *head = Tokenize();
    std::printf("Tokenizing done.\n");
    uint arg = 2;
    if (std::strcmp(argv[arg], "--visualize-token-stream") == 0) {
      arg++;
      std::printf("Generating graph file for token stream...\n");
//      if (arg < argc)
//        VisualizeTokenStream(head, argv[1]);
//      else VisualizeTokenStream(head, argv[arg++]);
      std::printf("Visualizing token stream done.\n");
    }
    std::printf("---------------------------------------------------------------------------\n");
    std::printf("Start parsing...\n");
    AstNode *ast = Parse(head);
    std::printf("Parsing done.\n");
    if (std::strcmp(argv[arg], "--visualize-ast") == 0) {
      arg++;
      if (arg < argc)
        VisualizeAst(ast, argv[1]);
      else VisualizeAst(ast, argv[arg++]);
    }
    std::printf("---------------------------------------------------------------------------\n");
    std::printf("Start executing...\n");
  }
}