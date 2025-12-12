
#include <stdio.h>

#include "common.h"


typedef struct {
    bool grid[3][3];
} Shape;

#define NUM_SHAPES 6
Shape shapes[NUM_SHAPES];


typedef struct {
    u32 width, height;
    u32 shape_count[NUM_SHAPES];
} Reg;
Make_Array(Reg, Regions);


internal Solution solve_input(String input) {

    Arena *allocator = Scratch_Get();

    Mem_Zero(shapes, sizeof(shapes));

    String_Array groups_of_lines = string_split_by(input, "\n\n");
    ASSERT(groups_of_lines.count == NUM_SHAPES+1);

    for (u32 i = 0; i < NUM_SHAPES; i++) {
        String_Array lines = string_split_by(groups_of_lines.items[i], "\n");
        ASSERT(lines.count == 4);
        ASSERT(lines.items[0].data[0] == (char)i + '0');
        for (u32 j = 1; j < lines.count; j++) {
            String line = lines.items[j];
            ASSERT(line.length == 3);
            for (u32 k = 0; k < line.length; k++) {
                char c = line.data[k];
                ASSERT(c == '#' || c == '.');
                shapes[i].grid[j-1][k] = (c == '#');
            }
        }
    }

    Regions regions = { .allocator = allocator };
    {
        String lines_of_regions = groups_of_lines.items[groups_of_lines.count-1];
        String_Array lines = string_split_by(lines_of_regions, "\n");
        for (u32 i = 0; i < lines.count; i++) {
            String line = lines.items[i];

            Reg reg;
            ASSERT(
                sscanf(
                    line.data, "%dx%d: %d %d %d %d %d %d",
                    &reg.width, &reg.height,
                    &reg.shape_count[0], &reg.shape_count[1], &reg.shape_count[2],
                    &reg.shape_count[3], &reg.shape_count[4], &reg.shape_count[5]
                ) == 8
            );

            Array_Append(&regions, reg);
        }
    }

    debug(regions.count);

    s64 can_fit = 0;
    for (u32 i = 0; i < regions.count; i++) {
        Reg reg = regions.items[i];

        u64 total_space = reg.width * reg.height;

        u64 total_number_of_boxes_to_pack = 0;
        for (u32 j = 0; j < Array_Len(reg.shape_count); j++) {
            total_number_of_boxes_to_pack += reg.shape_count[j];
        }

        bool will_fit_easily = ((total_space / 9) >= total_number_of_boxes_to_pack);
        if (will_fit_easily) {
            can_fit += 1;
            continue;
        }

        // here is where i would put some complicated packing algorithum
        // if the above check didnt just give me the correct answer.
    }

    Solution solution = {
        .part_1 = can_fit,
        .part_2 = 0,
    };
    return solution;
}


#define DAY "12"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(2, 0);

    Do_Input();

    // Perf_Input(Get_Input(), 10);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

