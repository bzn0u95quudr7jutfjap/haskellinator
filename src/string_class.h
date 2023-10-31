#include "stack.h"
#include <stdbool.h>
#include <string.h>

#ifndef STRING
#define STRING

DECLARE_STACK(char, String);

char *c_str(String *str);
bool equals(String *a, String *b);
String from_cstr(char *str);
void move_into(String *dst, String *src);
void String_delete(String *str);
bool is_empty(String *str);

#endif
