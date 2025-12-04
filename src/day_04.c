
#include <stdio.h>

#include "common.h"


Make_Array(bool, bool_Array);


typedef struct {
    _Array_Header_;
    bool *items;
    u32 width;
    u32 height;
} bool_grid;


// the grid is actually 1 bigger, so i can iterate a square easily
#define grid_index(grid, i, j)      ( (j+1) * ((grid)->width+1) + (i+1) )
#define grid_get(grid, i, j)  (grid)->items[grid_index((grid), (i), (j))]



#define Swap(a, b)          \
    do {                    \
        Typeof(a) tmp = a;  \
        a = b;              \
        b = tmp;            \
    } while (0)



internal bool is_not_surrounded(bool_grid *grid, s32 i, s32 j) {
    if (!grid_get(grid, i, j)) return false;

    u32 num_surrounding = 0;
    for (s32 k = -1; k <= +1; k++) {
        for (s32 l = -1; l <= +1; l++) {
            if (k == 0 && l == 0) continue;

            if (grid_get(grid, l+i, k+j)) num_surrounding += 1;
        }
    }

    return (num_surrounding < 4);
}


internal Solution solve_input(String input) {
    String_Array lines = string_split_by(input, "\n");

    Arena *allocator = Scratch_Get();
    bool_grid grid = { .allocator = allocator };
    grid.width  = lines.items[0].length;
    grid.height = lines.count;

    Array_Reserve(&grid, (grid.width+1)*(grid.height+1));
    grid.count = (grid.width+1)*(grid.height+1);



    // construct grid.
    for (u64 j = 0; j < lines.count; j++) {
        String line = lines.items[j];
        ASSERT(line.length == grid.width);
        for (u64 i = 0; i < line.length; i++) {
            char c = line.data[i];
            grid_get(&grid, i, j) = (c == '@');
        }
    }



    typedef struct {
        u16 i, j;
    } Position;

    Make_Array(Position, Position_Array);

    Position_Array hits_last_loop = { .allocator = allocator };
    Position_Array hits_next_loop = { .allocator = allocator };
    Array_Reserve(&hits_last_loop, 128);
    Array_Reserve(&hits_next_loop, 128);



    s64 num_total_removed = 0;
    s64 num_easy_accessed = 0;

    // inital loop. loop over everything
    for (s64 j = 0; j < grid.height; j++) {
        for (s64 i = 0; i < grid.width; i++) {
            if (!grid_get(&grid, i, j)) continue;

            if (is_not_surrounded(&grid, i, j)) {
                num_easy_accessed += 1;

                Position pos = {.i = i, .j = j};
                Array_Append(&hits_next_loop, pos);
            }
        }
    }




    // 66 iterations for input.
    while (true) {
        Swap(hits_last_loop, hits_next_loop);

        // clean grid
        for (u64 k = 0; k < hits_last_loop.count; k++) {
            Position pos = hits_last_loop.items[k];
            // clean duplicates.
            if (!grid_get(&grid, pos.i, pos.j)) {
                Array_Swap_And_Remove(&hits_last_loop, k);
                k -= 1;
            } else {
                grid_get(&grid, pos.i, pos.j) = false;
            }
        }

        num_total_removed += hits_last_loop.count;

        // iterate near the ones that were hit last loop.
        hits_next_loop.count = 0;
        for (u64 k = 0; k < hits_last_loop.count; k++) {
            Position pos = hits_last_loop.items[k];

            for (s32 k = -1; k <= +1; k++) {
                for (s32 l = -1; l <= +1; l++) {
                    if (k == 0 && l == 0) continue;

                    s32 i = l + pos.i;
                    s32 j = k + pos.j;

                    if (is_not_surrounded(&grid, i, j)) {
                        Position pos = {.i = i, .j = j};
                        Array_Append(&hits_next_loop, pos);
                    }
                }
            }
        }


        if (hits_next_loop.count == 0) break;
    }


    Solution solution = {
        .part_1 = num_easy_accessed,
        .part_2 = num_total_removed,
    };
    return solution;
}


#define DAY "04"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(13, 43);

    Do_Input();

    // Perf_Input(Get_Input(), 1000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

