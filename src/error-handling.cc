#include <error-handling.h>

void ThrowMsgError(Token* tk, ErrorType error, const char* msg) {
  tk->error_msg = msg;
  ThrowError(tk, error);
}

void ThrowError(Token* tk, ErrorType error) {
  if (tk == nullptr) exit(-1);
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
      std::printf("Unrecognizable character.\n");
      break;
    case MissingDeclarator:
      std::printf("Missing declarator.\n");
      break;
    case NotFunction:
      std::printf("Not a function.\n");
      break;
    case TokenExpected:
      std::printf("%s expected here.\n", tk->error_msg);
      break;
    case NullExpr:
      std::printf("Null expression.\n");
      break;
    case NullCond:
      std::printf("Null condition expression.\n");
      break;
    case IncompatibleType:
      std::printf("Incompatible type.\n");
      break;
    case NotSupported:
      std::printf("This syntax has not been supported yet.\n");
      break;
    case DimShouldBePosInt:
      std::printf("Dimensions in declarations should be positive integers.\n");
      break;
    case UnknownIdentifier:
      std::printf("Unknown identifier \"");
      for (uint i = 0; i < tk->len; i++) putchar(tk->loc[i]);
      std::printf("\".\n");
      break;
    case EmptyDeclarationStmt:
      std::printf("An empty declaration statement is not allowed.\n");
      break;
    case RepeatedDeclaration:
      std::printf("Repeated declaration of identifier \"");
      for (uint i = 0; i < tk->len; i++) putchar(tk->loc[i]);
      std::printf("\".\n");
      break;
    case LvalueExpected:
      std::printf("Left value expected at the left hand side.\n");
      break;
    case ComplexTypeInitNotAllowed:
      std::printf(
          "Declaring a complex type when initializing a for loop is not "
          "allowed.\n");
      break;
  }
  std::printf("%d| ", tk->line_no);
  uint line_no_width = 0;
  while (tk->line_no) {
    tk->line_no /= 10;
    line_no_width++;
  }
  PrintLine(tk->loc - tk->offset);
  for (int i = 0; i < tk->offset + line_no_width + 2; i++) putchar(' ');
  putchar('^');
  putchar('\n');
  exit(-1);
}