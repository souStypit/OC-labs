#include <stdio.h>

void del_member(int (*arr)[50], int len, int val) {
    if (val != len - 1) {
        (*arr)[val] = (*arr)[len - 1];
    }
}

int main() {
    int arr[50] = { 0, 1, 2, 3, 4, 5, 6 };
    int len = 7;
    for (int i = 0; i < len; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
    printf("\n");
    del_member(&arr, len--, 4);
    for (int i = 0; i < len; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
}