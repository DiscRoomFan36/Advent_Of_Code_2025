
#include <stdio.h>

#include "common.h"



#define get_grid(grid, i, j)    ( (grid).items[(j) * grid_width + (i)] )


internal Solution solve_input(String input) {

    Arena *allocator = Scratch_Get();


    u32 grid_width = 0;
    u32 grid_height;
    String_Array grid = { .allocator = allocator };


    u64 max_line_length = 0;

    String_Array lines = string_split_by(input, "\n", .dont_trim_lines = true, .allocator = allocator);
    grid_height = lines.count;

    for (u64 j = 0; j < lines.count; j++) {
        String line = lines.items[j];

        if (max_line_length < line.length)    max_line_length = line.length;

        String_Array line_split = string_split_by(line, " ", .allocator = allocator);
        for (u64 i = 0; i < line_split.count; i++) {
            String item = line_split.items[i];
            if (item.length == 0) {
                Array_Remove(&line_split, i, 1);
                i -= 1;
            }
        }

        if (grid_width == 0) {
            grid_width = line_split.count;
            debug(grid_width);
            Array_Add(&grid, grid_width*grid_height);
        }

        ASSERT(grid_width == line_split.count);

        for (u64 i = 0; i < line_split.count; i++) {
            get_grid(grid, i, j) = String_Duplicate(line_split.allocator, line_split.items[i], .null_terminate = true);
        }
    }


    s64 grand_total = 0;
    for (u64 i = 0; i < grid_width; i++) {
        char operator = get_grid(grid, i, grid_height-1).data[0];
        bool is_plus = (operator == '+');

        s64 this_total = is_plus ? 0 : 1;
        for (u64 j = 0; j < grid_height-1; j++) {
            String item = get_grid(grid, i, j);
            s64 number = atoll(item.data);
            if (is_plus) {
                this_total += number;
            } else {
                this_total *= number;
            }
        }

        grand_total += this_total;
    }



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

        if (index_of_spaces.count == grid_width-1) break;
    }


    // debug(index_of_spaces);

    s64 correct_total = 0;


    // for all the groups
    for (u64 i = 0; i < grid_width; i++) {
        char operator = get_grid(grid, i, grid_height-1).data[0];
        bool is_plus = (operator == '+');

        s64 this_total = is_plus ? 0 : 1;

        s64 offset     = i == 0            ? 0               : index_of_spaces.items[i-1] + 1;
        s64 next_space = i == grid_width-1 ? max_line_length : index_of_spaces.items[i];

        s32 collum_width = next_space - offset;

        // get one number
        for (s32 j = 0; j < collum_width; j++) {
            s64 this_spesific_number = 0;

            for (u64 k = 0; k < grid_height-1; k++) {
                u64 collum_index = j + offset;
                String line = lines.items[k];

                if (collum_index >= line.length) continue;

                char c = line.data[collum_index];
                if (c == ' ') continue;

                this_spesific_number = this_spesific_number * 10 + (c - '0');
            }

            if (is_plus) {
                this_total += this_spesific_number;
            } else {
                this_total *= this_spesific_number;
            }
        }

        correct_total += this_total;
    }


    Solution solution = {
        .part_1 = grand_total,
        .part_2 = correct_total,
    };
    return solution;
}


#define DAY "XX"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(4277556, 3263827);

    Do_Input();

    // Perf_Input(Get_Input(), 10);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

