#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
DECLARE_STACK(char, String);
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

DECLARE_STACK(String, Stack_String);
DEFINE_STACK(String, Stack_String);

String *get(Stack_String *stack, size_t i) {
  if (i < stack->size) {
    return &(stack->data[i]);
  }
  if (i >= -(stack->size)) {
    return &(stack->data[i + stack->size]);
  }
  return NULL;
}

char fpeekbackc(FILE *stream) {
  fseek(stream, -1, SEEK_CUR);
  return fgetc(stream);
}

char fpeekc(FILE *stream) {
  char c = fgetc(stream);
  fseek(stream, -1, SEEK_CUR);
  return c;
}

size_t fsize(FILE *stream) {
  size_t pos = ftell(stream);
  fseek(stream, 0, SEEK_END);
  size_t size = ftell(stream);
  fseek(stream, pos, SEEK_SET);
  return size;
}

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    fprintf(stdout, "  ");
  }
}

bool is_any_of(char c, size_t size, const char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

bool is_white(char c) {
  static const char *const charset = "\n \t";
  return is_any_of(c, strlen(charset), charset);
}

bool is_speciale(char c) {
  static const char *const charset = "<>{}()[]#.;,+-*/=&|";
  return is_any_of(c, strlen(charset), charset);
}

bool is_string_delimiter(char c) {
  static const char *const charset = "'\"";
  return is_any_of(c, strlen(charset), charset);
}

Stack_String parse_code_into_words(FILE *stream) {
  size_t pos = ftell(stream);
  fseek(stream, 0, SEEK_SET);

  Stack_String code = NewStack_String;
  push(&code, NewString);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    // commenti
    if (c == '/' && fpeekc(stream) == '/') {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      while ((c = fgetc(stream)) != EOF && c != '\n') {
        push(line, c);
      }
      push(&code, NewString);
      continue;
    }

    /* TODO TESTING
     * del COMMENTO multilinea
     */
    // commenti multilinea
    if (c == '/' && fpeekc(stream) == '*') {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      while ((c = fgetc(stream)) != EOF && !(c == '*' && fpeekc(stream) == '/')) {
        push(line, c);
      }
      push(line, c);
      push(line, fgetc(stream));
      push(&code, NewString);
      continue;
    }

    if (is_string_delimiter(c)) {
      char delimiter = c;
      String str = NewString;
      push(&str, delimiter);
      while ((c = fgetc(stream)) != EOF && c != delimiter) {
        if (c == '\\') {
          push(&str, c);
          push(&str, fgetc(stream));
        } else {
          push(&str, c);
        }
      }
      push(&str, delimiter);
      push(&code, str);
      push(&code, NewString);
      continue;
    }

    if (is_white(c)) {
      push(&code, NewString);
      continue;
    }

    if (is_speciale(c)) {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      push(&code, NewString);
      continue;
    }

    if (true) {
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      continue;
    }
  }

  fseek(stream, pos, SEEK_SET);
  return code;
}

Stack_String remove_empty_strings(Stack_String stack) {
  Stack_String filtered = NewStack_String;
  for (size_t i = 0; i < stack.size; i++) {
    String line = stack.data[i];
    if (line.size > 0) {
      push(&filtered, line);
    }
  }
  return filtered;
}

void append(String *a, String *b) {
  for (size_t i = 0; i < b->size; i++) {
    push(a, b->data[i]);
  }
}
void merge_include_macros(Stack_String *stack) {
  String cancelletto = from_cstr("#");
  String include = from_cstr("include");
  String lt = from_cstr("<");
  String gt = from_cstr(">");
  char *null_str = "null";

  String *ca = NULL;
  String *in = NULL;
  String *le = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    ca = get(stack, i);
    in = get(stack, i + 1);
    le = get(stack, i + 2);
    if (ca == NULL || in == NULL || le == NULL) {
      break;
    }
    if (!(equals(ca, &cancelletto) && equals(in, &include))) {
      continue;
    }

    if (le->data[0] == '"') {
      append(ca, in);
      push(ca, ' ');
      append(ca, le);
      in->size = 0;
      le->size = 0;
      i += 3;
      continue;
    }

    if (!equals(le, &lt)) {
      continue;
    }

    append(ca, in);
    push(ca, ' ');
    append(ca, le);
    in->size = 0;
    le->size = 0;
    i += 3;
    for (String *str = get(stack, i); str != NULL && !equals(str, &gt) && i < stack->size; i++) {
      append(ca, str);
      str->size = 0;
      str = get(stack, i + 1);
    }
    if (get(stack, i) == NULL) {
      break;
    }
    append(ca, get(stack, i));
    get(stack, i)->size = 0;
  }
}

void merge_parenthesis(Stack_String *stack) {
  String p_open = from_cstr("(");
  String p_closed = from_cstr(")");
  String coma = from_cstr(",");
  String *p = NULL;
  String *str = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    p = get(stack, i);
    if (p == NULL) {
      return;
    }
    if (p->size == 0 || !equals(p, &p_open)) {
      continue;
    }

    for (str = get(stack, ++i); str != NULL && !equals(str, &p_closed) && i < stack->size; i++) {
      if (equals(str, &coma)) {
        pop(p);
      }
      append(p, str);
      push(p, ' ');
      str->size = 0;
      str = get(stack, i + 1);
    }
    if (get(stack, i) == NULL) {
      break;
    }
    pop(p);
    append(p, get(stack, i));
    get(stack, i)->size = 0;
  }
}

bool is_operatore(String *str) { return str != NULL && str->size == 1 && is_speciale(str->data[0]); }

void merge_operatori(Stack_String *stack) {
  Stack_String ops = NewStack_String;
  push(&ops, from_cstr("<="));
  push(&ops, from_cstr(">="));
  push(&ops, from_cstr("=="));
  push(&ops, from_cstr("!="));
  push(&ops, from_cstr("&&"));
  push(&ops, from_cstr("||"));
  push(&ops, from_cstr("++"));
  push(&ops, from_cstr("--"));
  push(&ops, from_cstr("->"));
  push(&ops, from_cstr("+="));
  push(&ops, from_cstr("-="));
  push(&ops, from_cstr("*="));
  push(&ops, from_cstr("/="));
  push(&ops, from_cstr("&="));
  push(&ops, from_cstr("|="));
  String *str = NULL;
  String *str2 = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    str = get(stack, i);
    str2 = get(stack, i + 1);
    if (!is_operatore(str) || !is_operatore(str2)) {
      continue;
    }
    char comb[3] = {str->data[0], str2->data[0], 0};
    for (size_t j = 0; j < ops.size; j++) {
      if (strcmp(c_str(get(&ops, j)), comb) != 0) {
        continue;
      }
      if (strcmp("->", comb) == 0) {
        String *obj = get(stack, i - 1);
        String *attr = get(stack, i + 2);
        append(obj, str);
        append(obj, str2);
        append(obj, attr);
        str->size = 0;
        str2->size = 0;
        attr->size = 0;
        i += 2;
      } else {
        append(str, str2);
        i += 1;
        str2->size = 0;
      }
      break;
    }
  }
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "File da formattare non dato\n\nSINTASSI: %s <FILE>\n\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "File %s invalido\n\n", argv[1]);
    return 2;
  }

  Stack_String codeblocks = remove_empty_strings(parse_code_into_words(f));
  merge_include_macros(&codeblocks);
  merge_operatori(&codeblocks);
  //merge_parenthesis(&codeblocks);
  for (size_t i = 0; i < codeblocks.size; i++) {
    printf("%7zu : %s\n", i, c_str(get(&codeblocks, i)));
  }

  fclose(f);

  return 0;
}
