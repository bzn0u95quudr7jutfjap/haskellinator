#include "stack_string.h"
#include "string_class.h"

void merge_parenthesis_adjacent_rec(Stack_String *stack, String *line, size_t i, size_t j) {
  static void (*const this)(Stack_String *stack, String *line, size_t i, size_t j) = merge_parenthesis_adjacent_rec;
  if (i >= j) {
    return;
  }

  String *next = at(stack, i + 1);
  if (line == NULL || next == NULL) {
    return;
  }

  char *c = at(next, 0);
  if (c == NULL) {
    return this(stack, line, i + 1, j);
  }

  if (*c == ')' || *c == ',') {
    move_into(line, next);
  } else if (true) {
    push(line, ' ');
    move_into(line, next);
  }

  return this(stack, line, i + 1, j);
}

size_t find_parentesi_chiusa(Stack_String *stack, size_t i, size_t j) {
  if (j >= stack->size) {
    return j;
  }
  String *line = at(stack, j);
  if (line == NULL) {
    return j;
  }

  if (line->size > 0 && *at(line, 0) == '(') {
    return j;
  }

  if (line->size > 0 && *at(line, 0) == ')') {
    move_into(at(stack, i), at(stack, i + 1));
    merge_parenthesis_adjacent_rec(stack, at(stack, i), i + 2, j);
    // merge_parenthesis_adjacent_rec(stack, at(stack, i), i, j);
    return j;
  }

  return find_parentesi_chiusa(stack, i, j + 1);
}

void find_parentesi_aperta(Stack_String *stack, size_t i) {
  if (i >= stack->size) {
    return;
  }
  String *line = at(stack, i);
  if (line == NULL) {
    return;
  }

  if (line->size > 0 && *at(line, 0) == '(') {
    return find_parentesi_aperta(stack, find_parentesi_chiusa(stack, i, i + 1));
  }

  return find_parentesi_aperta(stack, i + 1);
}
