
#include <stdio.h>

#include "common.h"


internal Solution solve_input(String input) {
    String_Array lines = string_to_null_terminated_lines(input, true);

    s32 safe_number = 50;
    s64 number_of_0s_hit = 0;
    s64 number_of_perfect_0s = 0;

    for (u64 k = 0; k < lines.count; k++) {
        String line = lines.items[k];

        char l_or_r;
        s32 number;
        ASSERT(sscanf(line.data, "%c%d", &l_or_r, &number) == 2);


        s32 new_safe_number = safe_number;
        if (l_or_r == 'L') {
            new_safe_number -= number;

            for (s32 i = safe_number - 1; i >= new_safe_number; i -= 1) {
                if (i % 100 == 0) number_of_0s_hit += 1;
            }
        } else {
            new_safe_number += number;

            for (s32 i = safe_number + 1; i <= new_safe_number; i += 1) {
                if (i % 100 == 0) number_of_0s_hit += 1;
            }
        }

        safe_number = Proper_Mod(new_safe_number, 100);
        if (safe_number == 0) number_of_perfect_0s += 1;
    }

    Solution solution = {
        .part_1 = number_of_perfect_0s,
        .part_2 = number_of_0s_hit,
    };
    return solution;
}




#define DAY "01"


int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(3, 6);

    Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

