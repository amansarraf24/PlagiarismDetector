#include <stdio.h>

void g()
{
    int x = 10;
    int y = 20;
    int result = x + y;
    printf("Result: %d\n", result);
    return 0;
}

int main() {
    g();
}