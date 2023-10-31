#include "string_class.h"
#include <stdbool.h>
#include <string.h>

DEFINE_STACK(char, String);

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push(str, '\0');
  pop(str);
  return str->data;
}

bool equals(String *a, String *b) {
  return a != NULL && b != NULL && ((a == b) || ((a->size == b->size) && strcmp(c_str(a), c_str(b)) == 0));
}

String from_cstr(char *str) {
  String tmp = NewString;
  for (size_t i = 0; str[i]; i++) {
    push(&tmp, str[i]);
  }
  return tmp;
}

void String_delete(String * str){
  free(str->data);
  str->data = NULL;
  str->size = 0;
  str->capacity = 0;
}

bool is_empty(String * str){
  return str->size == 0;
}

void move_into(String *dst, String *src) {
  for (size_t i = 0; i < src->size; i++) {
    push(dst, src->data[i]);
  }
  String_delete(src);
}
