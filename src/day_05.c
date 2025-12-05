
#include <stdio.h>

#include "common.h"



internal Solution solve_input(String input) {

    Arena *allocator = Scratch_Get();

    Range_Array ranges    = { .allocator = allocator, };
    Int_Array ingredients = { .allocator = allocator, };


    String_Array lines = string_split_by(input, "\n");
    bool seen_empty_line = false;
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        if (line.length == 0) {
            seen_empty_line = true;
            continue;
        }

        if (!seen_empty_line) {
            // parse ranges
            Range range;
            ASSERT(sscanf(line.data, "%ld-%ld", &range.start, &range.end) == 2);
            Array_Append(&ranges, range);

        } else {
            // parse ingredients
            Array_Append(&ingredients, atoll(line.data));
        }
    }


    // sort ranges on start, and then merge them.
    qsort(ranges.items, ranges.count, sizeof(ranges.items[0]), compare_ranges_on_start);

    for (u64 i = 0; i < ranges.count-1; i++) {
        Range a = ranges.items[i];
        Range b = ranges.items[i+1];

        if (b.start <= a.end) {
            // merge them
            u64 new_end = Max(a.end, b.end);
            ranges.items[i].end = new_end;

            // this action moves the entire array
            Array_Remove(&ranges, i+1, 1);
            i -= 1;
        }
    }



    // part 1
    s64 fresh_ingredients = 0;
    for (u64 i = 0; i < ingredients.count; i++) {
        s64 id = ingredients.items[i];

        // binary_search
        s64 low = 0;
        s64 high = ranges.count-1;
        while (low < high) {
            s64 mid = low + (high - low) / 2;
            Range r = ranges.items[mid];

            if (r.start <= id && id <= r.end) {
                fresh_ingredients += 1;
                break;
            } else if (id < r.start) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }
    }


    // part 2
    s64 total_good_ingredients = 0;
    for (u64 i = 0; i < ranges.count; i++) {
        Range r = ranges.items[i];
        total_good_ingredients += (r.end - r.start) + 1;
    }


    Solution solution = {
        .part_1 = fresh_ingredients,
        .part_2 = total_good_ingredients,
    };
    return solution;
}


#define DAY "05"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(3, 14);

    Do_Input();

    // Perf_Input(Get_Input(), 5000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

