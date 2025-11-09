#include <stdio.h>
int zero(){return 0;}
int main() {
    int sum = zero();
    for(int i = zero(); i < 10; i++) {
        sum = sum + i;
    }
    printf("Sum: %d\n", sum);
    return 0;
}