
#include <stdio.h>

#include "common.h"


internal s64 solve_input(String input) {
    (void) input;

    s64 ints[] = {1, 2, 7, 3, 3, -1, 8, 2, 1, -65, 139, -75};
    Int_Array int_array = {.items = ints, .count = Array_Len(ints), .capacity = Array_Len(ints)};
    sort_int_array(&int_array);

    u64 y = 12;
    debug(y);
    debug(int_array.items[0]);
    String s = S("ballz");
    debug(s);
    debug(int_array);
    u64 *p_y = &y;
    debug(p_y);

    // debug_break();

    Int_Array new_int_array = static_array_to_array(Int_Array, ints);
    debug(new_int_array);


    return 0;
}


#define DAY "01"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    start_timer();
        s64 example = solve_input(Get_Example());
        printf("    example = %ld\n", example);
    finish_timer();

    // start_timer();
    //     s64 input   = solve_input(Get_Input());
    //     printf("    input   = %ld\n", input);
    // finish_timer();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

