#include "rb_struct.h"

rbtree *insertNode(int data) {
    rbtree *cur, *par, *x;

    cur = root;
    par = NULL;

    while (cur != nil) {
        if (data < cur->data) return cur;
        par = cur;
        cur = (data < cur->data) ? cur->left : cur->right;
    }

    if ((x = malloc(sizeof(rbtree))) == 0) {
        printf("Unsuccessfull memory allocation!");
        exit(1);
    }

    x->data = data;
    x->parent = par;
    x->left = nil;
    x->right = nil;
    x->color = red;

    if (par) {
        if (data < par->data)
            par->left = x;
        else
            par->right = x;
    } else {
        root = x;
    }

    insertFixup(x);
    return x;
}

void insertFixup(rbtree *x) {
    /*************************************
     *  maintain Red-Black tree balance  *
     *  after inserting node x           *
     *************************************/

    /* check Red-Black properties */
    while (x != root && x->parent->color == red) {
        /* we have a violation */
        if (x->parent == x->parent->parent->left) {
            rbtree *y = x->parent->parent->right;
            if (y->color == red) {
                /* uncle is RED */
                x->parent->color = black;
                y->color = black;
                x->parent->parent->color = red;
                x = x->parent->parent;
            } else {
                /* uncle is BLACK */
                if (x == x->parent->right) {
                    /* make x a left child */
                    x = x->parent;
                    rotateLeft(x);
                }

                /* recolor and rotate */
                x->parent->color = black;
                x->parent->parent->color = red;
                rotateRight(x->parent->parent);
            }
        } else {
            /* mirror image of above code */
            rbtree *y = x->parent->parent->left;
            if (y->color == red) {
                /* uncle is RED */
                x->parent->color = black;
                y->color = black;
                x->parent->parent->color = red;
                x = x->parent->parent;
            } else {
                /* uncle is BLACK */
                if (x == x->parent->left) {
                    x = x->parent;
                    rotateRight(x);
                }
                x->parent->color = black;
                x->parent->parent->color = red;
                rotateLeft(x->parent->parent);
            }
        }
    }
    root->color = black;
}

void deleteFixup(rbtree *x) {
    /*************************************
     *  maintain Red-Black tree balance  *
     *  after deleting node x            *
     *************************************/

    while (x != root && x->color == black) {
        if (x == x->parent->left) {
            rbtree *w = x->parent->right;
            if (w->color == red) {
                w->color = black;
                x->parent->color = red;
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == black && w->right->color == black) {
                w->color = red;
                x = x->parent;
            } else {
                if (w->right->color == black) {
                    w->left->color = black;
                    w->color = red;
                    rotateRight(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = black;
                w->right->color = black;
                rotateLeft(x->parent);
                x = root;
            }
        } else {
            rbtree *w = x->parent->left;
            if (w->color == red) {
                w->color = black;
                x->parent->color = red;
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == black && w->left->color == black) {
                w->color = red;
                x = x->parent;
            } else {
                if (w->left->color == black) {
                    w->right->color = black;
                    w->color = red;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = black;
                w->left->color = black;
                rotateRight(x->parent);
                x = root;
            }
        }
    }
    x->color = black;
}

void deleteNode(rbtree *z) {
    rbtree *x, *y;

    /*****************************
     *  delete node z from tree  *
     *****************************/

    if (!z || z == nil) return;

    if (z->left == nil || z->right == nil) {
        /* y has a NIL node as a child */
        y = z;
    } else {
        /* find tree successor with a NIL node as a child */
        y = z->right;
        while (y->left != nil) y = y->left;
    }

    /* x is y's only child */
    if (y->left != nil)
        x = y->left;
    else
        x = y->right;

    /* remove y from the parent chain */
    x->parent = y->parent;
    if (y->parent)
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    else
        root = x;

    if (y != z) z->data = y->data;

    if (y->color == black) deleteFixup(x);

    free(y);
}

rbtree *findNode(int data) {
    /*******************************
     *  find node containing data  *
     *******************************/

    rbtree *current = root;
    while (current != nil)
        if (data == current->data)
            return (current);
        else
            current = (data < current->data) ? current->left : current->right;
    return (0);
}

void rotateLeft(rbtree *x) {
    /**************************
     *  rotate node x to left *
     **************************/

    rbtree *y = x->right;

    /* establish x->right link */
    x->right = y->left;
    if (y->left != nil) y->left->parent = x;

    /* establish y->parent link */
    if (y != nil) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    } else {
        root = y;
    }

    /* link x and y */
    y->left = x;
    if (x != nil) x->parent = y;
}

void rotateRight(rbtree *x) {
    /****************************
     *  rotate node x to right  *
     ****************************/

    rbtree *y = x->left;

    /* establish x->left link */
    x->left = y->right;
    if (y->right != nil) y->right->parent = x;

    /* establish y->parent link */
    if (y != nil) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    } else {
        root = y;
    }

    /* link x and y */
    y->right = x;
    if (x != nil) x->parent = y;
}