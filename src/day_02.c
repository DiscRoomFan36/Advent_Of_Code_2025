
#include <stdio.h>

#include "common.h"


Make_Array(u8, u8_Array);

#define Array_Eq(array, i, j) ((array).items[(i)] == (array).items[(j)])


internal Solution solve_input(String input) {

    // replace (,) with (\n), so we can pass it though the split function.
    for (u32 i = 0; i < input.length; i++) {
        if (input.data[i] == ',') input.data[i] = '\n';
    }

    s64 invalid_id_sum = 0;
    s64 extra_invalid_id_sum = 0;

    u8_Array digits = ZEROED;
    digits.allocator = Scratch_Get();
    Array_Reserve(&digits, 32);

    String_Array lines = string_to_null_terminated_lines(input, true);
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        u64 start, end;
        ASSERT(sscanf(line.data, "%ld-%ld", &start, &end) == 2);


        for (u64 i = start; i <= end; i++) {
            // total_iterations = 1943348

            digits.count = 0;

            // put the digits into an array.
            u64 n = i;
            while (n > 0) {
                u64 div = n / 10;
                u64 mod = n % 10;

                Array_Append(&digits, mod);
                n = div;
            }


            // part 1
            if (digits.count % 2 == 0) {
                u32 group_size = digits.count/2;

                // looping backwards is faster, checks the first digit faster.
                bool the_same = true;
                for (s32 j = group_size - 1; j >= 0; j--) {
                    if (!Array_Eq(digits, j, j + group_size)) {
                        the_same = false;
                        break;
                    }
                }

                if (the_same) {
                    invalid_id_sum       += i;
                    extra_invalid_id_sum += i;
                    continue;
                }
            }


            // handle 3 or more.
            //
            // j == number of groups
            for (u32 j = 3; j <= digits.count; j++) {
                // the patern won't fit into the array perfectly
                if (digits.count % j != 0) continue;

                u32 group_size = digits.count / j;
                bool is_same = true;

                // check all the groups are the same.
                for (s32 l = j-2; l >= 0; l--) {
                    for (s32 m = group_size-1; m >= 0; m--) {
                        if (!Array_Eq(digits, l * group_size + m, (l+1) * group_size + m)) {
                            is_same = false;
                            goto continue_ounter_loop;
                        }
                    }
                }

                if (is_same) {
                    extra_invalid_id_sum += i;
                    break; // dont check more sizes
                }
                continue_ounter_loop:;
            }

        }
    }

    Solution solution = {
        .part_1 = invalid_id_sum,
        .part_2 = extra_invalid_id_sum,
    };
    return solution;
}


#define DAY "02"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(1227775554, 4174379265);

    Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

