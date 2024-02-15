#include <stdio.h>
#include "NDL.h"

int main() {
    long start = NDL_GetTicks();
    int cont = 100000;
    while (cont>=0) {
        long end = NDL_GetTicks();
        long elapsed = end - start;
        if (elapsed >= 500) {
            printf("0.5 seconds have passed.\n");
            start = end;
        }
        cont--;
    }
    return 0;
}

