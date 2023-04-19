#include "rb_struct.h"

rbtree sentinel = {nil, nil, NULL, black, 0};
rbtree *root = nil;

int main() {
    insertNode(10);
    insertNode(10 * 2);

    printf("root -> %d %d\n", root->data, root->color);
    return 0;
}