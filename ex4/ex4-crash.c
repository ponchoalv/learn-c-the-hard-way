#include <stdio.h>


void crash() {
    char* pepe = NULL;

    printf("hey %c", pepe[0]);
}

int main()
{
    int age = 10;
    int height = 72;

    printf("I am %d years old.\n", age);
    printf("I am %d inchses tall.\n", height);

    crash();

    return 0;
}