#include <stdio.h>
#include <unistd.h>

int main() {
    while (1) {
        printf("Я занимаю всю командную строку!\n");
        sleep(5);
    }
}