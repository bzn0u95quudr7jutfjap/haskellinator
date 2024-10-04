#ifndef STRING
#define STRING

#include "stack.h"
#include <int.h>
#include <stddef.h>

declare_template_stack_c(char, String);
declare_template_stack_c(String, Stack_String);

typedef enum {
  TOKEN_COMMENT_SL,
  TOKEN_COMMENT_ML,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_OPERATOR,
  TOKEN_SPECIAL,
  TOKEN_MACRO_BEGIN,
  TOKEN_MACRO_END,
  TOKEN_TONDA_BEGIN,
  TOKEN_TONDA_END,
  TOKEN_QUADRA_BEGIN,
  TOKEN_QUADRA_END,
  TOKEN_GRAFFA_BEGIN,
  TOKEN_GRAFFA_END,
} token_type;

typedef struct {
  token_type type;
  String *str;
} Token;

declare_template_stack_c(Token, Stack_Token);

extern Stack_Token g_tokens;

u8 is_any_of(char c, size_t size, const char cs[]);
Stack_String tokenizer(String * stream);

char *c_str(String *str);
String from_cstr(char *str);
void move_into(String *dst, String *src);
void String_delete(String *str);

u8 equals_string(String *a, String *b);

typedef struct {
  String *str;
  u32 idx;
  u32 is_end;
  char val;
} Iter_String;

Iter_String sseekres(String *stream);
char sgetc(Iter_String *stream);
char speekc(Iter_String *stream);
char speekoffset(Iter_String *stream, int o);
void sseekcur(Iter_String *stream, i32 o);

#endif
