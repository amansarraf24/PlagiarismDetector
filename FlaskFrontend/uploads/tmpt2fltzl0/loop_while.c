#include <stdio.h>

int main() {
    int total = 0;
    int j = 0;
    while(j < 10) {
        total = total * j;
        j++;
    }
    printf("Product: %d\n", total);
    return 0;
}