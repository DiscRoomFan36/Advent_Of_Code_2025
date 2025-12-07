
#include <stdio.h>

#include "common.h"


internal Solution solve_input(String input) {

    String_Array lines = string_split_by(input, "\n");
    u32 beam_start_pos;
    {
        bool flag = false;
        for (u64 i = 0; i < lines.items[0].length; i++) {
            char c = lines.items[0].data[i];
            if (c == 'S') {
                beam_start_pos = i;
                flag = true;
                break;
            }
        }
        ASSERT(flag);
    }



    u64 curr_beam_array[512];
    u64 next_beam_array[512];

    // this is why we have assert's kids.
    u32 array_width = lines.items[0].length;
    ASSERT(array_width <= Array_Len(curr_beam_array));


    Mem_Zero(curr_beam_array, sizeof(curr_beam_array));
    Mem_Zero(next_beam_array, sizeof(next_beam_array));

    curr_beam_array[beam_start_pos] += 1;
    next_beam_array[beam_start_pos] += 1;


    s64 num_splits = 0;

    for (u64 j = 1; j < lines.count; j++) {
        String line = lines.items[j];
        ASSERT(line.length == array_width);

        for (u64 i = 0; i < array_width; i++) {
            u64 num = curr_beam_array[i];
            if (num == 0) continue;

            char c = line.data[i];

            if (c == '^') {
                num_splits += 1;

                next_beam_array[i-1] += num;
                next_beam_array[i  ]  =   0;
                next_beam_array[i+1] += num;
            }
        }

        Mem_Copy(curr_beam_array, next_beam_array, array_width * sizeof(next_beam_array[0]));
    }


    s64 quantum_beam_count = 0;
    for (u32 i = 0; i < array_width; i++) {
        u64 num = curr_beam_array[i];
        quantum_beam_count += num;
    }



    Solution solution = {
        .part_1 = num_splits,
        .part_2 = quantum_beam_count,
    };
    return solution;
}


#define DAY "07"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(21, 40);

    Do_Input();

    // Perf_Input(Get_Input(), 10000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

