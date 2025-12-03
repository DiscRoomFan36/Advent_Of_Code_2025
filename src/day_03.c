
#include <stdio.h>

#include "common.h"



internal s64 find_largest_jolts(Int_Array array, u32 depth) {
    s64 result = 0;
    while (depth != 0) {
        s64 max_num   = 0;
        u32 max_index = 0;
        for (u32 i = 0; i <= array.count - depth; i++) {
            s64 x = array.items[i];
            if (x == 9) {
                max_num   = x;
                max_index = i;
                break;
            }

            if (max_num < x) {
                max_num   = x;
                max_index = i;
            }
        }

        array.count -= max_index + 1;
        array.items += max_index + 1;
        result = result * 10 + max_num;
        depth -= 1;
    }

    return result;
}



internal Solution solve_input(String input) {

    Arena *allocator = Scratch_Get();
    Int_Array digits = { .allocator = Scratch_Get() };

    s64 total_jolts = 0;
    s64 total_super_jolts = 0;

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        digits.count = 0;
        for (u64 i = 0; i < line.length; i++) {
            char c = line.data[i];
            Array_Append(&digits, c - '0');
        }

        total_jolts       += find_largest_jolts(digits,  2);
        total_super_jolts += find_largest_jolts(digits, 12);
    }

    Solution solution = {
        .part_1 = total_jolts,
        .part_2 = total_super_jolts,
    };
    return solution;
}


#define DAY "03"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(357, 3121910778619);

    Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

