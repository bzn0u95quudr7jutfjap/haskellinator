#include "stack.h"
#include "stack_string.h"

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

