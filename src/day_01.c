
#include <stdio.h>

#include "common.h"


#define Proper_Mod(x, y) ((((x) % (y)) + (y)) % (y))

internal s64 solve_input(String input) {
    (void) input;

    s32 safe_number = 50;
    s64 number_of_0s_hit = 0;
    u64 num = 0;

    while (true) {
        String line = String_Get_Next_Line(&input, &num, SGNL_All);
        if (line.length == 0) break;

        line.data[line.length] = 0;
        s32 number = atoi(line.data + 1);
        ASSERT(number != 0);

        if (line.data[0] == 'L') {
            safe_number -= number;
        } else {
            safe_number += number;
        }

        safe_number = Proper_Mod(safe_number, 100);
        if (safe_number == 0) number_of_0s_hit += 1;
    }


    return number_of_0s_hit;
}


internal s64 solve_part_2(String input) {
    s32 safe_number = 50;
    s64 number_of_0s_hit = 0;
    u64 num = 0;
    // debug_break();

    while (true) {
        String line = String_Get_Next_Line(&input, &num, SGNL_All);
        if (line.length == 0) break;

        line.data[line.length] = 0;
        s32 number = atoi(line.data + 1);
        ASSERT(number != 0);



        s32 new_safe_number = safe_number;
        if (line.data[0] == 'L') {
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
    }


    return number_of_0s_hit;
}


#define DAY "01"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    start_timer();
        s64 example = solve_input(Get_Example());
        printf("    example = %ld\n", example);
    finish_timer();

    start_timer();
        s64 example_part_2 = solve_part_2(Get_Example());
        printf("    example_part_2 = %ld\n", example_part_2);
    finish_timer();


    start_timer();
        s64 input   = solve_input(Get_Input());
        printf("    input   = %ld\n", input);
    finish_timer();

    start_timer();
        s64 input_part_2 = solve_part_2(Get_Input());
        printf("    input_part_2 = %ld\n", input_part_2);
    finish_timer();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

