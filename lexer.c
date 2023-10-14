#include <ctype.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  _ILLEGAL,
  _EOF,
  _IDENT,
  _INT,
  _ASSIGN,
  _PLUS,
  _COMMA,
  _SEMICOLON,
  _LPAREN,
  _RPAREN,
  _LBRACE,
  _RBRACE,
  _FUNCTION,
  _LET,
} token_type;

typedef struct {
  token_type type;
  char* literal;
  size_t literal_length;
} token;

typedef struct {
  char* input;
  size_t input_length;
  size_t position;
  size_t read_position;
  char ch;
} lexer;

token* token_make(token_type type, char* literal);

lexer* lexer_make(char* input);
void lexer_read_char(lexer* l);
char* lexer_read_identifier(lexer* l);
char* lexer_read_number(lexer* l);
token* lexer_next_token(lexer* l);
void lexer_skip_whitespace(lexer* l);

token_type lookup_identifier(char* literal);

bool is_letter(char ch);
bool is_digit(char ch);

char buffer[100];

void TEST_one(void) {
  char* input = "=+(){},;";

  token tests[9] = {
    {.type = _ASSIGN, .literal="="},
    {.type = _PLUS, .literal="+"},
    {.type = _LPAREN, .literal="("},
    {.type = _RPAREN, .literal=")"},
    {.type = _LBRACE, .literal="{"},
    {.type = _RBRACE, .literal="}"},
    {.type = _COMMA, .literal=","},
    {.type = _SEMICOLON, .literal=";"},
    {.type = _EOF, .literal=""},
  };

  lexer* l = lexer_make(input);

  for (size_t i = 0; i < 9; i++) {
    token* tok = lexer_next_token(l);

    if (tok->type != tests[i].type) {
      printf("tests[%zu] - tokentype wrong. expected=%u, got=%u\n", i, tests[i].type, tok->type);
    }

    if (strncmp(tok->literal, tests[i].literal, tok->literal_length) != 0) {
      printf("tests[%zu] - literal wrong. expected=%u, got=%u\n", i, tests[i].type, tok->type);
    }

    free(tok->literal);
    free(tok);
  }

  free(l);
}

void TEST_two(void) {
  char* input = "let five = 5;"
    "let ten = 10;"
    "let add = fn(x, y) {"
    "x + y;"
    "};"
    "let result = add(five, ten);";

  token tests[37] = {
    {.type=_LET, .literal="let"},
    {.type=_IDENT, .literal="five"},
    {.type=_ASSIGN, .literal="="},
    {.type=_INT, .literal="5"},
    {.type=_SEMICOLON, .literal=";"},
    {.type=_LET, .literal="let"},
    {.type=_IDENT, .literal="ten"},
    {.type=_ASSIGN, .literal="="},
    {.type=_INT, .literal="10"},
    {.type=_SEMICOLON, .literal=";"},
    {.type=_LET, .literal="let"},
    {.type=_IDENT, .literal="add"},
    {.type=_ASSIGN, .literal="="},
    {.type=_FUNCTION, .literal="fn"},
    {.type=_LPAREN, .literal="("},
    {.type=_IDENT, .literal="x"},
    {.type=_COMMA, .literal=","},
    {.type=_IDENT, .literal="y"},
    {.type=_RPAREN, .literal=")"},
    {.type=_LBRACE, .literal="{"},
    {.type=_IDENT, .literal="x"},
    {.type=_PLUS, .literal="+"},
    {.type=_IDENT, .literal="y"},
    {.type=_SEMICOLON, .literal=";"},
    {.type=_RBRACE, .literal="}"},
    {.type=_SEMICOLON, .literal=";"},
    {.type=_LET, .literal="let"},
    {.type=_IDENT, .literal="result"},
    {.type=_ASSIGN, .literal="="},
    {.type=_IDENT, .literal="add"},
    {.type=_LPAREN, .literal="("},
    {.type=_IDENT, .literal="five"},
    {.type=_COMMA, .literal=","},
    {.type=_IDENT, .literal="ten"},
    {.type=_RPAREN, .literal=")"},
    {.type=_SEMICOLON, .literal=";"},
    {.type=_EOF, .literal=""},
  };

  lexer* l = lexer_make(input);

  for (size_t i = 0; i < 37; ++i) {
    token* tok = lexer_next_token(l);

    if (tok->type != tests[i].type) {
      printf("tests[%zu] - tokentype wrong. expected=%u, got=%u\n", i, tests[i].type, tok->type);
    }

    if (strncmp(tok->literal, tests[i].literal, tok->literal_length) != 0) {
      printf("tests[%zu] - literal wrong. expected=%u, got=%u\n", i, tests[i].type, tok->type);
    }

    free(tok->literal);
    free(tok);
  }

  free(l);
}

int main(void) {
  TEST_one();
  TEST_two();
  return 0;
}

token* token_make(token_type type, char* literal) {
  token* t = malloc(sizeof(token));
  size_t len = strlen(literal);

  t->literal = (char *)malloc(sizeof(len) + 1);
  t->type = type;
  t->literal_length = len;

  strncpy(t->literal, literal, len);

  return t;
}

lexer* lexer_make(char* input) {
  lexer* l = malloc(sizeof(lexer));

  l->input = input;
  // TODO
  // got an error here with input_length coming back as too many or something
  // like that
  l->input_length = strlen(input);

  lexer_read_char(l);

  return l;
}

void lexer_read_char(lexer* l) {
  if (l->read_position >= l->input_length) {
    l->ch = '\0';
  } else {
    l->ch = l->input[l->read_position];
  }
  l->position = l->read_position;
  l->read_position += 1;
}

char* lexer_read_identifier(lexer* l) {
  size_t position = l->position;

  while (is_letter(l->ch)) {
    lexer_read_char(l);
  }

  size_t length = l->position - position;

  buffer[0] = '\0';
  strncpy(buffer, l->input + position, length);
  buffer[length] = '\0';

  return buffer;
}

char* lexer_read_number(lexer* l) {
  size_t position = l->position;

  while (is_digit(l->ch)) {
    lexer_read_char(l);
  }

  size_t length = l->position - position;

  buffer[0] = '\0';
  strncpy(buffer, l->input + position, length);
  buffer[length] = '\0';

  return buffer;
}

token* lexer_next_token(lexer* l) {
  token* t = NULL;

  lexer_skip_whitespace(l);

  switch (l->ch) {
    case '=':
      t = token_make(_ASSIGN, &l->ch);
      break;
    case ';':
      t = token_make(_SEMICOLON, &l->ch);
      break;
    case '(':
      t = token_make(_LPAREN, &l->ch);
      break;
    case ')':
      t = token_make(_RPAREN, &l->ch);
      break;
    case ',':
      t = token_make(_COMMA, &l->ch);
      break;
    case '+':
      t = token_make(_PLUS, &l->ch);
      break;
    case '{':
      t = token_make(_LBRACE, &l->ch);
      break;
    case '}':
      t = token_make(_RBRACE, &l->ch);
      break;
    case '\0':
      t = token_make(_EOF, "");
      break;
    default:
      if (is_letter(l->ch)) {
        char* lit = lexer_read_identifier(l);
        t = token_make(lookup_identifier(lit), lit);
        return t;
      } else if (is_digit(l->ch)) {
        char* lit = lexer_read_number(l);
        t = token_make(_INT, lit);
        return t;
      } else {
        t = token_make(_ILLEGAL, &l->ch);
      }
      break;
  }

  lexer_read_char(l);
  return t;
}

void lexer_skip_whitespace(lexer* l) {
  switch (l->ch) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      lexer_read_char(l);
      break;
  }
}

token_type lookup_identifier(char* literal) {
  if (strncmp("fn", literal, strlen(literal)) == 0) {
    return _FUNCTION;
  }

  if (strncmp("let", literal, strlen(literal)) == 0) {
    return _LET;
  }

  return _IDENT;
}

bool is_letter(char ch) {
  if (isalpha(ch) || ch == '_') return true;
  return false;
}

bool is_digit(char ch) {
  return isdigit(ch);
}
