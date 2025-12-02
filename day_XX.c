
#include <stdio.h>

#include "common.h"


typedef struct {
    char charater;
    s64 number;
} Input_Line;
Make_Array(Input_Line, Input_Line_Array)


internal Solution solve_input(String input) {

    Input_Line_Array inputs = { .allocator = Scratch_Get() };


    String_Array lines = string_split_by(input, ",");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Input_Line input_line;
        ASSERT(sscanf(line.data, "%c%d", &input_line.character, &input_line.number) == 2);
        Array_Append(&inputs, input_line);
    }

    for (u64 inputs_index = 0; inputs_index < inputs.count; inputs_index++) {
        Input_Line input_line = inputs.items[inputs_index];
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

