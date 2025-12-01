
#include <stdio.h>

#include "common.h"


internal Solution solve_input(String input) {
    String_Array lines = string_to_null_terminated_lines(input, true);

    for (u64 k = 0; k < lines.count; k++) {
        String line = lines.items[k];

        char character;
        s32 number;
        ASSERT(sscanf(line.data, "%c%d", &character, &number) == 2);
    }

    Solution solution = {
        .part_1 = 0,
        .part_2 = 0,
    };
    return solution;
}


#define DAY "XX"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(0, 0);

    // Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

