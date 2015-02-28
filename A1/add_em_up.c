#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
    int num, sum = 0;

    if (argc < 2) {
        return 1;
    }

    while (scanf("%d", &num) == 1) {
        if (strcmp("neg", argv[1]) == 0) {
            if (num < 0) {
                sum += num;
            }
        } else if (strcmp("pos", argv[1]) == 0) {
            if (num > 0) {
                sum += num;
            }
        } else {
            return 1;
        }
    }

    printf("Sum of %s integers in %s is: %d", argv[1], argv[2], sum);

    return 0;   
}