
#include <stdio.h>

#include "common.h"




internal bool number_is_invalid_id_group_2(u64 n) {
    u32 log_10 = int_log_10(n);
    u64 pow_10 = int_pow(10, log_10/2);
    return (n % pow_10) == (n / pow_10);
}



internal Solution solve_input(String input) {

    Range_Array ranges = { .allocator = Scratch_Get() };

    String_Array lines = string_split_by(input, ",");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Range range;
        ASSERT(sscanf(line.data, "%ld-%ld", &range.start, &range.end) == 2);

        Array_Append(&ranges, range);
    }

    s64 max_end = 0;
    for (u32 i = 0; i < ranges.count; i++) {
        if (max_end < ranges.items[i].end) max_end = ranges.items[i].end;
    }
    u32 max_log_10 = int_log_10(max_end);


    Int_Array all_invalid_ids = { .allocator = Scratch_Get() };
    {
        u64 j_start  = 0;
        u64 mask_mul = 1;
        u64 i_end    = 0;

        for (u32 k = 0; k < max_log_10/2; k++) {
            j_start  += 1;
            mask_mul *= 10;

            u64 i_start = int_pow(10, k);
            i_end       = i_end * 10 + 9;

            u64 mask = 1;
            for (u64 j = j_start; j <= max_log_10; j += j_start) {
                mask = mask * mask_mul + 1;

                for (u64 i = i_start; i <= i_end; i++) {
                    s64 y = i * mask;
                    if (y > max_end) break;
                    Array_Append(&all_invalid_ids, y);
                }
            }
        }

    }
    sort_int_array(&all_invalid_ids);


    s64 invalid_id_sum = 0;
    s64 extra_invalid_id_sum = 0;

    for (u32 i = 0; i < ranges.count; i++) {
        Range range = ranges.items[i];

        u64 index = binary_search(all_invalid_ids, range.start);

        for (u64 k = index; k < all_invalid_ids.count; k++) {
            s64 id = all_invalid_ids.items[k];
            if (id > range.end) break;

            // handle duplicates
            if ((k < all_invalid_ids.count-1) && (id == all_invalid_ids.items[k+1])) continue;


            if (number_is_invalid_id_group_2(id)) invalid_id_sum += id;
            extra_invalid_id_sum += id;
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

