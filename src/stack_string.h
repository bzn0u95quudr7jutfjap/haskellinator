#include "stack.h"
#include "string_class.h"

#ifndef STACK_STRING
#define STACK_STRING

DECLARE_STACK(String, Stack_String);

String * get(Stack_String * stack, size_t i);

#endif 
