#include <error-handling.h>

void ThrowMsgError(Token* tk, ErrorType error, const char *msg) {
  tk->error_msg = msg;
  ThrowError(tk, error);
}

void ThrowError(Token* tk, ErrorType error) {
  if(tk == nullptr) exit(-1);
  std::printf("Error at line %d:%d: ", tk->line_no, tk->offset + 1);
  switch (error) {
    case TokenTooLong:
      std::printf("Token is too long.\n");
      break;
    case InvalidToken:
      std::printf("Token is invalid.\n");
      break;
    case InvalidFile:
      std::printf("Unrecognizable character.\n");
      break;
    case FailedMemStream:
      std::printf("Failed to open memory stream. Stop compiling.\n");
      exit(-1);
    case UnrecognizableCharacter:
      std::printf("Unrecognizable character");
      break;
    case MissingDeclarator:
      std::printf("Missing declarator");
      break;
    case NotFunction:
      std::printf("Not a function");
      break;
    case TokenExpected:
      std::printf("%s expected here", tk->error_msg);
      break;
    default:
      std::printf("Compile Error.\n");
      break;
  }
  std::printf("%d| ", tk->line_no);
  uint line_no_width = 0;
  while(tk->line_no) {
    tk->line_no /= 10;
    line_no_width++;
  }
  PrintLine(tk->loc - tk->offset);
  for (int i = 0; i < tk->offset + line_no_width + 2; i++) putchar(' ');
  putchar('^');
  putchar('\n');
  exit(-1);
}