
#include <stdio.h>

#include "common.h"





internal bool number_is_invalid_id_group_2(u64 n) {
    // we dont explicently check that 'log_10 % 2 == 0'
    // but the return statement will always fail if that is the case
    u32 log_10 = int_log_10(n)+1;
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
    u32 max_log_10 = int_log_10(max_end)+1;


    // Get all possible invalid id's, (that are smaller than max_end)
    Int_Array all_invalid_ids = { .allocator = Scratch_Get() };

    {
        Arena *allocator = Scratch_Get();
        Int_Array array_array[32] = ZEROED;
        u32 array_array_count = 0;

        u64 j_start  = 0;
        u64 mask_mul = 1;
        u64 i_end    = 0;

        for (u32 k = 0; k < max_log_10/2; k++) {
            j_start  += 1;
            mask_mul *= 10;

            u64 i_start = int_pow(10, k);
            i_end       = i_end * 10 + 9;

            Int_Array array = { .allocator = allocator };
            // reserve all you need. might be more than you need
            Array_Reserve(&array, (max_log_10 - j_start) * (i_end - i_start + 1));

            u64 mask = 1;
            for (u64 j = j_start; j <= max_log_10; j += j_start) {
                mask = mask * mask_mul + 1;

                for (u64 i = i_start; i <= i_end; i++) {
                    s64 y = i * mask;
                    if (max_end < y) break;
                    array.items[array.count++] = y;
                }
            }

            array_array[array_array_count++] = array;
            ASSERT(array_array_count <= Array_Len(array_array));
        }

        // array_array is an array of arrays,
        //
        // all the arrays are sorted locally, now we just need to merge them.
        //
        // min heap might be slower than just checking every one every single time.
        // theres only like 5 of them

        u64 total_elements = 0;
        for (u64 i = 0; i < array_array_count; i++) total_elements += array_array[i].count;

        // put them into the final array,
        Array_Reserve(&all_invalid_ids, total_elements);
        all_invalid_ids.count = total_elements;

        struct Stack_Item {
            s64 number;
            u32 array_index;
            u32 element_index;
        } stack[Array_Len(array_array)] = ZEROED;
        u32 stack_count = array_array_count;
        for (u64 i = 0; i < stack_count; i++) {
            struct Stack_Item item = {
                .number        = array_array[i].items[0],
                .array_index   = i,
                .element_index = 0,
            };
            stack[i] = item;
        }



        u64 invalid_ids_index = 0;
        while (stack_count > 0) {
            // find smallest element TODO min heap.
            struct Stack_Item *smallest = &stack[0];
            for (u32 i = 1; i < stack_count; i++) {
                if (stack[i].number < smallest->number) {
                    smallest = &stack[i];
                }
            }

            all_invalid_ids.items[invalid_ids_index++] = smallest->number;
            smallest->element_index += 1;


            Int_Array *a = &array_array[smallest->array_index];

            // remove if the array is out of elements
            if (smallest->element_index >= a->count) {
                // stamp and remove.
                if (smallest != &stack[stack_count-1]) {
                    *smallest = stack[stack_count-1];
                }
                stack_count -= 1;
            } else {
                smallest->number = a->items[smallest->element_index];
            }

        }
    }



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

    // Perf_Input(Get_Input(), 1000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

