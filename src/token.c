#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <stack.h>
#include <stack_string.h>
#include <string_class.h>
#include <token.h>

size_t string_index;

void sseekres() { string_index = 0; }

char sgetc(String *stream) {
  char *c = at(stream, string_index++);
  return c == NULL ? EOF : *c;
}

char speekc(String *stream) {
  char *c = at(stream, string_index);
  return c == NULL ? EOF : *c;
}

char speekoffset(String *stream, int o) {
  char *c = at(stream, string_index + o);
  return c == NULL ? EOF : *c;
}

void sseekcur(int o) { string_index += o; }

bool is_any_of(char c, size_t size, const char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

#define DEFINE_CSET(NAME, CSET)                                                                                        \
  bool NAME(char c) {                                                                                                  \
    static const char *const charset = CSET;                                                                           \
    return is_any_of(c, strlen(charset), charset);                                                                     \
  }

DEFINE_CSET(is_white, " \n\t")
DEFINE_CSET(is_special, "{}()[]#.;,")
DEFINE_CSET(is_string_delimiter, "'\"")
DEFINE_CSET(is_operator, "+-*/%!=&|^><")

bool is_name_first(char c) { return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
bool is_name(char c) { return is_name_first(c) || ('0' <= c && c <= '9'); }
bool is_number1_first(char c) { return ('0' <= c && c <= '9'); }
bool is_number1(char c) {
  return is_number1_first(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || c == 'L' || c == 'x' || c == 'X';
}

String *pushNewString(Stack_String *tokens) {
  push(tokens, NewString);
  return at(tokens, -1);
}

Stack_String tokenizer(String *stream) {
  char c = EOF;
  Stack_String tokens_obj = NewStack_String;
  Stack_String *tokens = &tokens_obj;

  for (sseekres(); (c = speekc(stream)) != EOF;) {
    if (is_name_first(c)) {
      String *token = pushNewString(tokens);
      while (is_name(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(-1);
    } else if (is_special(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
    } else if (is_operator(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      if (is_operator(c = sgetc(stream))) {
        push(token, c);
      } else {
        sseekcur(-1);
      }
    } else if (is_number1(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while (is_number1(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(-1);
    } else if (is_string_delimiter(c)) {
      char delimiter = c;
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while ((c = sgetc(stream)) != EOF && c != delimiter && c != '\n') {
        push(token, c);
        if (c == '\\') {
          push(token, sgetc(stream));
        }
      }
      if (c == delimiter) {
        push(token, c);
      } else {
        sseekcur(-1);
      }
    } else {
      sgetc(stream);
    }
  }
  return tokens_obj;
}

Stack_String parse_code_into_words(FILE *stream) {
  String s = NewString;
  fseek(stream, 0, SEEK_SET);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    push(&s, c);
  }
  return tokenizer(&s);
}
