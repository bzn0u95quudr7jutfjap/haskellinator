#include "stack.h"

#define TREE(T, TREE)                                                                              \
  typedef struct TREE TREE;                                                                        \
  STACK(TREE *, Stack_##TREE);                                                                     \
  struct TREE {                                                                                    \
    T data;                                                                                        \
    Stack_##TREE childs;                                                                           \
  }
