#include <error-handling.h>
#include <hash.h>

#include <cctype>
#include <cstdio>

static const uint kPgSize = 4096;

static Token* head = nullptr;

static char* read_ptr;
static char* begin_of_line;
static uint line_no = 1;
static char* buffer;
static size_t buffer_size = kPgSize;

static bool IsReserved(Token* tk) {
  static char reserved_list[][10] = {
      "var", "if", "else", "for", "spm", "vec", "return", "break", "continue"
  };
  static GeneralHashMap<9, 1331, uint> reserved_hash;
  if (reserved_hash.empty()) {  // init reserved_hash
    for (uint i = 0; i < 9; i++)
      reserved_hash.insert(reserved_list[i], i);
  }
  return reserved_hash.find(tk->loc, tk->len);
}

/**
 * identifier = alpha(alpha | digit)*
 * @return
 */
static void ReadIdentifierOrReserved(Token* tk) {
  while (isalpha(*read_ptr) || isdigit(*read_ptr)) {
    read_ptr++;
    tk->len++;
  }
  if (IsReserved(tk)) tk->type = TK_Reserved;
  else tk->type = TK_Identifier;
}

static int ch2hex(char c) {
  if (isdigit(c))
    return c - '0';
  else if (isupper(c) && c <= 'F')
    return c - 'A' + 10;
  else if (islower(c) && c <= 'f')
    return c - 'a' + 10;
  else
    return -1;
}

static int ch2oct(char c) {
  if (isdigit(c) && c < '8')
    return c - '0';
  else
    return -1;
}

static int ch2dec(char c) {
  if (isdigit(c))
    return c - '0';
  else
    return -1;
}

static int ReadDecInt(Token* tk) {
  int x = 0;
  while (isdigit(*read_ptr)) {
    if (ch2dec(*read_ptr) < 0) ThrowError(tk, InvalidToken);
    x = (x << 3) + (x << 1) + ch2dec(*read_ptr);
    read_ptr++;
  }
  return x;
}

static int ReadOctInt(Token* tk) {
  int x = 0;
  while (isdigit(*read_ptr)) {
    if (ch2oct(*read_ptr) < 0) ThrowError(tk, InvalidToken);
    x = (x << 3) + ch2oct(*read_ptr);
    read_ptr++;
  }
  return x;
}

static int ReadHexInt(Token* tk) {
  int x = 0;
  while (isdigit(*read_ptr) || isalpha(*read_ptr)) {
    if (ch2hex(*read_ptr) < 0) ThrowError(tk, InvalidToken);
    x = (x << 4) + ch2hex(*read_ptr);
    read_ptr++;
  }
  return x;
}

static Real ReadRealPart(Token* tk) {
  int real_part_i = ReadDecInt(tk);
  Real real_part = static_cast<Real>(real_part_i);
  if (real_part_i) {
    int base = 1;
    while (base <= real_part_i) {
      base = (base << 3) + (base << 1);
      real_part *= 0.1;
    }
  }
  return real_part;
}

static int ReadSignInt(Token* tk) {
  bool sgn = false;
  if (*read_ptr == '-') {
    sgn = true;
    read_ptr++;
  } else if (*read_ptr == '+')
    read_ptr++;
  int x = ReadDecInt(tk);
  return sgn ? -x : x;
}

static Real Pow10(Real base, int x) {
  Real mul = x > 0 ? 10 : 0.1;
  if (x < 0) x = -x;
  for (int i = 0; i < x; i++) base *= mul;
  return base;
}

void ReadNumber(Token* tk) {
  char* cur = read_ptr;
  bool read_hex = false;
  bool read_real = false;
  bool read_oct = false;
  int int_num;
  int exp_num = 0;
  Real real_part = 0.0;
  if (*read_ptr == '0' && *(read_ptr + 1) == 'x') {
    read_ptr += 2;
    int_num = ReadHexInt(tk);
    read_hex = true;
  } else if (*(tk->loc) == '0' && isdigit(*(read_ptr + 1))) {
    read_ptr++;
    int_num = ReadOctInt(tk);
    read_oct = true;
  } else {
    int_num = ReadDecInt(tk);
  }
  if (*read_ptr == '.') {
    if (read_hex || read_oct) ThrowError(tk, InvalidToken);
    read_ptr++;
    real_part = ReadRealPart(tk);
    read_real = true;
  }
  if (*read_ptr == 'e' || *read_ptr == 'E') {
    if (read_hex || read_oct) ThrowError(tk, InvalidToken);
    read_ptr++;
    exp_num = ReadSignInt(tk);
    read_real = true;
  }
  tk->len = read_ptr - cur;
  if (read_real) {
    tk->real_num = Pow10((static_cast<Real>(int_num) + real_part), exp_num);
    tk->type = TK_Real;
  } else {
    tk->type = TK_Int;
    tk->int_num = int_num;
  }
}

void ReadPunct(Token* tk) {
  static char punct_list[][4] = {
      "<<=", ">>=", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=",
      "++",  "--",  "%=", "&=", "|=", "^=", "&&", "||", "<<", ">>",
  };
  static GeneralHashMap<24, 1331, uint> punct_hash;
  if (punct_hash.empty()) {  // init punct_hash
    for (uint i = 0; i < sizeof(punct_list) / (sizeof(char) << 2); i++)
      punct_hash.insert(punct_list[i], i);
  }
  int i = 3;
  for (; i >= 2; i--)
    if (punct_hash.find(read_ptr, i)) break;
  tk->len = i;
  tk->type = TK_Punct;
  read_ptr += i;
}

/**
 * Read a token
 * TODO: add support for comments
 * If this is called, then the blanks must be read previously
 * @return pointer to the new token
 */
static Token* ReadToken() {
  auto tk = new Token;
  tk->line_no = line_no;
  tk->offset = read_ptr - begin_of_line;
  tk->loc = read_ptr;
  char c = *read_ptr;
  if (isalpha(c)) {
    ReadIdentifierOrReserved(tk);
  }
  else if (isdigit(c))
    ReadNumber(tk);
  else if (ispunct(c))
    ReadPunct(tk);
  else if (c == EOF || c == '\0')
    tk->type = TK_Eof;
  else
    ThrowError(tk, UnrecognizableCharacter);
  return tk;
}

void SkipSpaces() {
  while (isspace(*read_ptr)) {
    if (*read_ptr == '\n') {
      line_no++;
      begin_of_line = read_ptr + 1;
    }
    read_ptr++;
  }
}

Token* Scan() {
  SkipSpaces();
  Token* tk = ReadToken();
  return tk;
}

size_t ReadCode(const char* file) {
  static char buf[4096];
  FILE* fp = fopen(file, "r");
  if (!fp) ThrowError(head, InvalidFile);
  FILE* mem_stream = open_memstream(&buffer, &buffer_size);
  if (!mem_stream) ThrowError(head, FailedMemStream);
  size_t tot = 0;
  while (true) {
    size_t n = fread(buf, sizeof(char), kPgSize, fp);
    fwrite(buf, sizeof(char), n, mem_stream);
    tot += n;
    if (n < kPgSize) break;
  }
  fflush(mem_stream);
  if (tot && buffer[tot - 1] != '\n')
    fputc('\n', mem_stream);  // let EOF lie on a new line will be convenient
  fclose(mem_stream);
  read_ptr = buffer;
  return tot;
}

Token* Tokenize() {
  begin_of_line = buffer;
  head = Scan();
  Token* tail = head;
  while (tail->type != TK_Eof) {
    Token* tk = Scan();
    tail->nxt = tk;
    tail = tk;
  }
  return head;
}

/**
 * For debugging tokenizer
 */
void PrintTokenStream() {
  Token* tk = head;
  while (tk->type != TK_Eof) {
    PrintToken(tk);
    tk = tk->nxt;
  }
  PrintToken(tk);
}