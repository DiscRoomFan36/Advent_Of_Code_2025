
#include <stdio.h>

#include "common.h"


internal s64 solve_input(String input) {
    (void) input;
    return 0;
}


#define DAY "XX"

int main(void) {
    printf("Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    s64 example = solve_input(Get_Example());
    printf("    example = %ld\n", example);

    // s64 input   = solve_input(Get_Input());
    // printf("    input   = %ld\n", input);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

