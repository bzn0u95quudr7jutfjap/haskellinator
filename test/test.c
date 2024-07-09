#include "../src/stack_string.h"
#include "../src/string_class.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void assertTrue(char *name, bool b) { printf("Testing :: [%3s] : %s\n", b ? "OK" : "ERR", name); }

int test_tokenization(char *name, Stack_String (*function)(String *), String *input, Stack_String *output) {
  Stack_String a = function(input);
  bool errcode = 0;
  int min = 0;
  errcode += (a.size == output->size) != true;
  for (int i = 0; i < a.size && i < output->size; i++) {
    errcode += !equals(&(a.data[i]), &(output->data[i]));
  }
  assertTrue(name, errcode == 0);
  return errcode;
}

String *new_string(char *src) {
  String i = from_cstr(src);

  String *s = malloc(sizeof(*s));
  memcpy(&s, &i, sizeof(void *));
  return s;
}

Stack_String *new_stack_string(int argc, char *argv[]) {
  Stack_String ss = NewStack_String;
  for (int i = 0; i < argc; i++) {
    push(&ss, from_cstr(argv[i]));
  }

  Stack_String *s = malloc(sizeof(*s));
  memcpy(&s, &ss, sizeof(void *));
  return s;
}

#define test_tokenization(name, function, string, stack)                                                               \
  char *name##_argv[] = stack;                                                                                         \
  String name##_string = from_cstr(string);                                                                            \
  Stack_String name##_stack = NewStack_String;                                                                         \
  for (int i = 0; i < (sizeof(name##_argv) / sizeof(name##_argv[0])); i++) {                                           \
    push(&name##_stack, from_cstr(name##_argv[i]));                                                                    \
  }                                                                                                                    \
  test_tokenization(#name, function, &name##_string, &name##_stack)

Stack_String id(String *a) { return NewStack_String; };

int main(int argc, char *argv[]) {
  test_tokenization(token_prova, id, "", {});
  test_tokenization(token_commenti, id, "", {});
  test_tokenization(token_stringhe, id, "", {});
  test_tokenization(token_funzione, id, "", {});
  return 0;
}
