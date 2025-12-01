
#include <stdio.h>

#include "common.h"


#define Proper_Mod(x, y) ((((x) % (y)) + (y)) % (y))

typedef struct {
    s64 part_1;
    s64 part_2;
} Solution;

internal Solution solve_input(String input) {
    s32 safe_number = 50;
    s64 number_of_0s_hit = 0;
    s64 number_of_perfect_0s = 0;
    u64 num = 0;

    while (true) {
        String line = String_Get_Next_Line(&input, &num, SGNL_All);
        if (line.length == 0) break;


        line.data[line.length] = 0;

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



#define Do_Example(part_1_solution, part_2_solution)                                                    \
    do {                                                                                                \
        start_timer();                                                                                  \
            Solution example = solve_input(Get_Example());                                              \
        finish_timer();                                                                                 \
                                                                                                        \
        printf("    example:\n");                                                                       \
        printf("        part 1: %18ld,    correct: %18ld\n", example.part_1, (s64)part_1_solution);     \
        printf("        part 2: %18ld,    correct: %18ld\n", example.part_2, (s64)part_2_solution);     \
    } while (0)


#define Do_Input()                                              \
    do {                                                        \
        start_timer();                                          \
            Solution input = solve_input(Get_Input());          \
        finish_timer();                                         \
                                                                \
        printf("    input  :\n");                               \
        printf("        part 1: %18ld\n", input.part_1);        \
        printf("        part 2: %18ld\n", input.part_2);        \
    } while (0)



int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(3, 6);

    Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

