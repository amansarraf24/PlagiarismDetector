#include <stdio.h>

int main() {
    int total = 1;
    int j = 1;
    while(j < 10) {
        total = total * j;
        j++;
    }
    printf("Product: %d\n", total);
    return 0;
}