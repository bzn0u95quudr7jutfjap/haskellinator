#include "stack.h"

#define MAKE_TREE(T, TREE)                                                                         \
  typedef struct TREE TREE;                                                                        \
                                                                                                   \
  STACK(TREE *, Stack_##TREE##_p);                                                                 \
                                                                                                   \
  struct TREE {                                                                                    \
    T data;                                                                                        \
    Stack_tree_p childs;                                                                           \
  };
