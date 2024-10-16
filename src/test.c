#include "test.h"
#include "string_class.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

u0 exec_test(testdata *t) { t->vtable.exec(t); }
u0 print_test(testdata *t) { t->vtable.print(t); }
u0 free_test(testdata *t) { t->vtable.free(t); }

define_test(
    test_tokenizer,
    {
      t->tokenizer.function(&t->tokenizer.input);
      t->code = equals_formatter_strings(&t->tokenizer.input, &t->tokenizer.atteso);
    },
    {
      printf("%6s :: %s\n", "INIZIO", t->name);
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Atteso");
      print_stack_string(&t->tokenizer.atteso);
      printf("\n");
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Output");
      print_formatter(&t->tokenizer.input);
      printf("\n");
      switch (t->code) {
      case CODE_NULL_RESULT:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_NULL_RESULT");
        printf("%6s :: [%3s] :: %10s :: %p\n", "STATUS", status(t), "Atteso", &t->tokenizer.atteso);
        printf("%6s :: [%3s] :: %10s :: %p\n", "STATUS", status(t), "Output", &t->tokenizer.input);
        break;
      case CODE_TOKEN_STR_DIFFER:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_TOKEN_STR_DIFFER");
        break;
      case CODE_TOKEN_LEN_DIFFER:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_TOKEN_LEN_DIFFER");
        printf("%6s :: [%3s] :: %10s :: LEN : %6lu\n", "STATUS", status(t), "Atteso", t->tokenizer.atteso.size);
        printf("%6s :: [%3s] :: %10s :: LEN : %6lu\n", "STATUS", status(t), "Output", t->tokenizer.input.tokens.size);
        break;
      default:
        break;
      }
      printf("%6s :: %s\n", "FINE", t->name);
    },
    {
      free_Formatter(&t->tokenizer.input);
      free_Stack_String(&t->tokenizer.atteso);
    });

u8 success(testdata *t) { return t->code == CODE_OK; }
char *status(testdata *t) { return success(t) ? "OK" : "ERR"; }

u0 print_string(String *s) {
  String buffer = new_String();
  resize(&buffer, s->size * 2);
  for (u32 i = 0; i < s->size; i++) {
    char c = *at_String(s, i);
    if (32 < c && c < 127) {
      push_String(&buffer, c);
    } else {
      u8 g = c;
      switch (g) {
      case ' ':
        push_String(&buffer, g);
        break;
      case '\t':
        push_String(&buffer, '\\');
        push_String(&buffer, 't');
        break;
      case '\n':
        push_String(&buffer, '\\');
        push_String(&buffer, 'n');
        break;
      case '\r':
        push_String(&buffer, '\\');
        push_String(&buffer, 'r');
        break;
      default:
        push_String(&buffer, '\\');
        push_String(&buffer, 'x');
        push_String(&buffer, '0' + (g / 16));
        push_String(&buffer, '0' + (g % 16));
        break;
      };
    }
  }
  printf("{%s}", c_str(&buffer));
  free_String(&buffer);
}

u0 print_stack_string(Stack_String *output) {
  for (u32 i = 0; i < output->size; i++) {
    print_string(at(output, i));
  }
}

u0 print_formatter(Formatter *fmt) {
  String view = {};
  for (u32 i = 0; i < fmt->tokens.size; i++) {
    Token *t = at(&fmt->tokens, i);
    view.size = t->size;
    view.data = fmt->str.data + t->begin;
    print_string(&view);
  }
}

testcode equals_string(String *a, String *b) {
  if (a == b) {
    return CODE_OK;
  }
  if (a == NULL || b == NULL) {
    return CODE_STR_NULL_RESULT;
  }
  if (a->size != b->size) {
    return CODE_TOKEN_STR_LEN_DIFFER;
  }
  for (u32 i = 0; i < a->size; i++) {
    if (strncmp(a->data, b->data, a->size) != 0) {
      return CODE_TOKEN_STR_DIFFER;
    }
  }
  return CODE_OK;
}

testcode equals_formatter_strings(Formatter *a, Stack_String *b) {
  if (a == NULL || b == NULL) {
    return CODE_NULL_RESULT;
  }
  if (a->tokens.size != b->size) {
    return CODE_TOKEN_LEN_DIFFER;
  }
  for (u32 i = 0; i < a->tokens.size; i++) {
    Token *t = at(&a->tokens, i);
    String view = {.size = t->size, .data = a->str.data + t->begin};
    testcode c = equals_string(&view, at(b, i));
    if (c != CODE_OK) {
      return c;
    }
  }
  return CODE_OK;
}

String file_get_content(char *fn) {
  String tmp = new_String();
  char c = EOF;
  FILE *f = fopen(fn, "r");
  if (f == NULL) {
    return tmp;
  }
  while ((c = fgetc(f)) != EOF) {
    push_String(&tmp, c);
  }
  fclose(f);
  return tmp;
}

char *type_string(token_type t) {
  switch (t) {
  case TOKEN_IDENTIFIER:
    return "ID";
    break;
  case TOKEN_NUMBER:
    return "NUM";
    break;
  case TOKEN_STRING:
    return "STR";
    break;
  case TOKEN_COMMENT_SL:
    return "CMSL";
    break;
  case TOKEN_COMMENT_ML:
    return "CMML";
    break;
  case TOKEN_OPERATOR:
    return "OP";
    break;
  case TOKEN_SPECIAL:
    return "SPE";
    break;
  case TOKEN_MACRO_BEGIN:
    return "MACRO B";
    break;
  case TOKEN_MACRO_END:
    return "MACRO E";
    break;
  }
}

Stack_String ss_ca(char *strings[], u32 len) {
  Stack_String ss1 = new_Stack_String();
  for (int i = 0; i < len; i++) {
    push_Stack_String(&ss1, from_cstr(strings[i]));
  }
  return ss1;
}
