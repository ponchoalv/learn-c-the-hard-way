#include "libex28.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void) {
    assert(strncmp(hello_world(), "Hello World", 20) == 0 && "The result must be Hello World");
    printf("%s", hello_world());
    return 0;
}

