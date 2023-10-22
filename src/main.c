#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

STACK(char);
typedef Stack_char String;
void append(String * stack, String * data){
  for(size_t i = 0; i < data->size; i++){
    Stack_char_push(stack,data->data[i]);
  }
}

enum STATE { VIRGOLA, COMMENTO, MACRO, GRAFFA_A, GRAFFA_B };

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

bool is_any_of(char c, size_t size, char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

bool is_white(char c) { return is_any_of(c, 3, "\n \t"); }

bool is_operatore(char c) { return is_any_of(c, 10, "+-*/<>&|=!"); }

void consume_while_white(FILE *stream) {
  char c;
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); is_white(c = fgetc(stream)) && i < file_len; i++) {
  }
  fseek(stream, -1, SEEK_CUR);
}

void print_operatore(char c, FILE *stream, bool comma) {
  char next = fpeekc(stream);
  if (is_operatore(next)) {
    if (c == next) {
      if (c == '+' || c == '-') {
        if (!comma && !is_white(fpeekbackc(stream))) {
          fprintf(stdout, "%c%c ", c, fgetc(stream));
        } else {
          fprintf(stdout, " %c%c", c, fgetc(stream));
        }
      } else {
        fprintf(stdout, " %c%c ", c, fgetc(stream));
      }
    } else {
      fprintf(stdout, " %c%c ", c, fgetc(stream));
    }
  } else {
    if (comma) {
      fprintf(stdout, "%c ", c);
    } else {
      fprintf(stdout, " %c ", c);
    }
  }
}

void print_inside_quote(FILE *stream, char delimiter) {
  char c = fpeekbackc(stream);
  if (c != delimiter) {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '%c'\n", c, delimiter);
    return;
  }

  fprintf(stdout, "%c", delimiter);
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); (c = fgetc(stream)) != delimiter && i < file_len; i++) {
    if (c == '\\') {
      fprintf(stdout, "\\%c", fgetc(stream));
    } else {
      fprintf(stdout, "%c", c);
    }
  }
  fprintf(stdout, "%c", delimiter);
}

void format_parenthesis(FILE *stream) {
  char c = fpeekbackc(stream);
  if (c != '(') {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '('\n", c);
    return;
  }

  fprintf(stdout, "(");
  size_t file_len = fsize(stream);
  bool comma = false;
  for (size_t i = ftell(stream); (c = fgetc(stream)) != ')' && i < file_len; i++) {
    if (c == '(') {
      format_parenthesis(stream);
    } else if (c == '"' || c == '\'') {
      print_inside_quote(stream, c);
    } else if (is_operatore(c)) {
      print_operatore(c, stream, comma);
    } else if (is_white(c)) {
      consume_while_white(stream);
      continue; // skip comma = false;
    } else if (c == ',') {
      fprintf(stdout, ", ");
      comma = true;
      continue; // skip comma = false;
    } else if (is_white(c)) {
      consume_while_white(stream);
      fprintf(stdout, " ");
    } else {
      fprintf(stdout, "%c", c);
    }
    comma = false;
  }
  fprintf(stdout, ")");
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

  char c = 0;
  int indentation_level = 0;
  size_t file_len = fsize(f);
  enum STATE state;
  for (int i = 0; (c = fgetc(f)) != EOF && i < file_len + 12; i++) {

    if (c == '#') {
      if (state != MACRO) {
        fprintf(stdout, "\n");
      }
      state = MACRO;
      fprintf(stdout, "#");
      while (!((c = fgetc(f)) != '\\' && fpeekc(f) == '\n')) {
        fprintf(stdout, "%c", c);
      }
      consume_while_white(f);
      fprintf(stdout, "%c\n", c);
      print_indentation(indentation_level);
      continue;
    }

    if (c == '}') {
      do {
        consume_while_white(f);
        fprintf(stdout, "\n");
        print_indentation(indentation_level);
        fprintf(stdout, "%c", c);
        indentation_level--;
      } while ((c = fgetc(f)) == '}');
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      if (indentation_level == 0) {
        fprintf(stdout, "\n");
      }
      fseek(f, -1, SEEK_CUR);
      continue;
    }

    if (c == '/' && fpeekc(f) == '/') {
      if (state != COMMENTO) {
        fprintf(stdout, "\n");
        print_indentation(indentation_level);
      }
      state = COMMENTO;
      fprintf(stdout, "/");
      while ((c = fgetc(f)) != '\n') {
        fprintf(stdout, "%c", c);
      }
      consume_while_white(f);
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      continue;
    }

    if (c == '"' || c == '\'') {
      print_inside_quote(f, c);
      //    } else if (is_white(c)) {
      //      char next = fpeekc(f);
      //      if (is_operatore(next)) {
      //      }else if(is_white(next)){
      //        printf(" ");
      //        consume_while_white(f);
      //      }else{
      //        printf(" ");
      //      }
    } else if (is_operatore(c)) {
      print_operatore(c, f, false);
    } else if (c == '(') {
      format_parenthesis(f);
    } else if (c == ';') {
      consume_while_white(f);
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      fprintf(stdout, "%c ", c);
      state = VIRGOLA;
    } else if (c == '{') {
      indentation_level++;
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      fprintf(stdout, "%c ", c);
      consume_while_white(f);
    } else {
      fprintf(stdout, "%c", c);
    }
  }

  fclose(f);

  return 0;
}
