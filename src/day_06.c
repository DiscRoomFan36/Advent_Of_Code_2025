
#include <stdio.h>

#include "common.h"



internal Solution solve_input(String input) {

    Arena *allocator = Scratch_Get();



    u64 max_line_length = 0;

    String_Array lines = string_split_by(input, "\n", .dont_trim_lines = true, .allocator = allocator);
    u32 grid_height = lines.count;

    for (u64 j = 0; j < lines.count; j++) {
        String line = lines.items[j];
        if (max_line_length < line.length)    max_line_length = line.length;
    }

    // collect all the collums, by knowing all the correct boundrys
    Int_Array index_of_spaces = { .allocator = allocator };
    for (u64 i = 0; i < lines.items[0].length; i++) {
        char c = lines.items[0].data[i];
        if (c == ' ') Array_Append(&index_of_spaces, i);
    }

    for (u64 j = 1; j < lines.count; j++) {
        String line = lines.items[j];
        for (u64 i = 0; i < index_of_spaces.count; i++) {
            char c = line.data[index_of_spaces.items[i]];
            if (c != ' ') {
                Array_Remove(&index_of_spaces, i, 1);
                i -= 1;
            }
        }
    }


    s64 left_to_right_total = 0;
    s64 right_to_left_and_top_to_bottom_total = 0;


    // for all the groups
    for (u64 i = 0; i < index_of_spaces.count+1; i++) {

        s32 offset     = i == 0                     ? 0               : index_of_spaces.items[i-1] + 1;
        s32 next_space = i == index_of_spaces.count ? max_line_length : index_of_spaces.items[i];

        s32 collum_width = next_space - offset;


        bool is_plus;
        {
            bool flag = false;
            for (s32 j = offset; j < next_space; j++) {
                char c = lines.items[lines.count-1].data[j];
                if (c == '+' || c == '*') {
                    is_plus = (c == '+');
                    flag = true;
                    break;
                }
            }
            ASSERT(flag);
        }


        { // part 1
            s64 this_total = is_plus ? 0 : 1;

            for (u64 k = 0; k < grid_height-1; k++) {
                String line = lines.items[k];
                s32 number = atoi(&line.data[offset]);

                if (is_plus) {
                    this_total += number;
                } else {
                    this_total *= number;
                }
            }
            left_to_right_total += this_total;
        }


        { // part 2
            // get one number
            s64 this_total = is_plus ? 0 : 1;

            for (s32 j = 0; j < collum_width; j++) {
                s64 this_specific_number = 0;

                for (u64 k = 0; k < grid_height-1; k++) {
                    u64 collum_index = j + offset;
                    String line = lines.items[k];

                    if (collum_index >= line.length) continue;

                    char c = line.data[collum_index];
                    if (c == ' ') continue;

                    this_specific_number = this_specific_number * 10 + (c - '0');
                }

                if (is_plus) {
                    this_total += this_specific_number;
                } else {
                    this_total *= this_specific_number;
                }
            }

            right_to_left_and_top_to_bottom_total += this_total;
        }

    }


    Solution solution = {
        .part_1 = left_to_right_total,
        .part_2 = right_to_left_and_top_to_bottom_total,
    };
    return solution;
}


#define DAY "06"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(4277556, 3263827);

    Do_Input();

    // Perf_Input(Get_Input(), 1000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

