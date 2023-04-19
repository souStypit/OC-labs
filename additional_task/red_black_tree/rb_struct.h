#include <stdio.h>
#include <stdlib.h>

typedef enum { black, red } colors;

typedef struct red_black_tree {
    struct red_black_tree *left, *right, *parent;
    colors color;
    int data;
} rbtree;

#define nil &sentinel
extern rbtree sentinel;
extern rbtree *root;

rbtree *insertNode(int data);
void insertFixup(rbtree *x);
void deleteFixup(rbtree *x);
void deleteNode(rbtree *z);
rbtree *findNode(int data);
void rotateLeft(rbtree *x);
void rotateRight(rbtree *x);