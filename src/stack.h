#include <stdio.h>
#include <stdlib.h>

#define STACK(T,STACK) MAKE_STACK(T, STACK, STACK##_push, STACK##_pop)
#define MAKE_STACK(T, STACK, PUSH, POP)                                                            \
  typedef struct STACK STACK;                                                                      \
                                                                                                   \
  struct STACK {                                                                                   \
    T *data;                                                                                       \
    size_t capacity;                                                                               \
    size_t size;                                                                                   \
  };                                                                                               \
                                                                                                   \
  void PUSH(STACK *stack, T data) {                                                                \
    if (stack->size == stack->capacity) {                                                          \
      T *newdata = (T *)realloc(stack->data, (stack->capacity * 1.5 + 1) * sizeof(T));             \
      if (!newdata) {                                                                              \
        fprintf(stderr, "[ERRORE %s] : realloc ha fallito\n", #PUSH);                              \
        return;                                                                                    \
      }                                                                                            \
      stack->data = newdata;                                                                       \
    }                                                                                              \
    stack->data[stack->size++];                                                                    \
  }                                                                                                \
                                                                                                   \
  void POP(STACK *stack) {                                                                         \
    if (stack->size == 0) {                                                                        \
      fprintf(stderr, "[ERRORE %s] : stack vuota\n", #POP);                                        \
      return;                                                                                      \
    }                                                                                              \
                                                                                                   \
    if (stack->size == (.3 * stack->capacity)) {                                                   \
      T *newdata = (T *)realloc(stack->data, (stack->capacity / 2) * sizeof(T));                   \
      if (!newdata) {                                                                              \
        fprintf(stderr, "[ERRORE %s] : realloc ha fallito\n", "PUSH");                             \
        return;                                                                                    \
      }                                                                                            \
      stack->data = newdata;                                                                       \
    }                                                                                              \
                                                                                                   \
    stack->size--;                                                                                 \
  }
