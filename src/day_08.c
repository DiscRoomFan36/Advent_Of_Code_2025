
#include <stdio.h>

#include "common.h"


typedef struct {
    s32 x, y, z;
} Vector3;
Make_Array(Vector3, Vector3_Array);


void print_Vector3(Vector3 v) {
    printf("%d,%d,%d", v.x, v.y, v.z);
}

void print_Vector3_Array(Vector3_Array array) {
    printf("{\n");
    for (u64 i = 0; i < array.count; i++) {
        printf("    ");
        print_Vector3(array.items[i]);
        printf(",\n");
    }
    printf("}");
}


// must be 64 bit, input numbers are two high.
internal inline s64 dist_sqr(Vector3 a, Vector3 b) {
    s64 x = a.x-b.x;
    s64 y = a.y-b.y;
    s64 z = a.z-b.z;
    return x*x + y*y + z*z;
}


typedef u16 Index_Type;

typedef struct {
    Index_Type index_a;
    Index_Type index_b;
    f32 dist_sqr; // f32 is smaller than s64, now this entire struct fits in a 64-bit number
} Index_And_Dist_Pair;
Make_Array(Index_And_Dist_Pair, Distance_Pairs_Array);



internal int compare_dists_and_pairs(const void *_a, const void *_b) {
    Index_And_Dist_Pair *a = (Index_And_Dist_Pair*)_a;
    Index_And_Dist_Pair *b = (Index_And_Dist_Pair*)_b;
    if      (a->dist_sqr < b->dist_sqr)     return -1;
    else if (a->dist_sqr > b->dist_sqr)     return  1;
    else                                    return  0;
}

internal int compare_Index_Types(const void *_a, const void *_b) {
    Index_Type a = *(Index_Type*)_a;
    Index_Type b = *(Index_Type*)_b;
    if      (a > b)  return -1;
    else if (a < b)  return  1;
    else             return  0;
}



internal Solution solve_input(String input) {
    Arena *allocator = Scratch_Get();
    Vector3_Array junction_boxs = { .allocator = allocator };

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Vector3 box;
        ASSERT(sscanf(line.data, "%d,%d,%d", &box.x, &box.y, &box.z) == 3);
        Array_Append(&junction_boxs, box);
    }

    // print_Vector3_Array(junction_boxs);
    u32 num_connections_to_make = junction_boxs.count == 20 ? 10 : 1000;

    // index type should be able to index every junction box.
    ASSERT( junction_boxs.count < (1UL << (sizeof(Index_Type)*8-1)) );


    Distance_Pairs_Array dists_and_pairs = { .allocator = allocator };

    {
        u32 number_of_pairs = (junction_boxs.count * (junction_boxs.count-1)) / 2;
        Array_Add(&dists_and_pairs, number_of_pairs);
        u32 pairs_count = 0;

        for (u64 i = 0; i < junction_boxs.count-1; i++) {
            Vector3 a = junction_boxs.items[i];
            for (u64 j = i+1; j < junction_boxs.count; j++) {
                Vector3 b = junction_boxs.items[j];

                s64 dist = dist_sqr(a, b);
                f32 dist_f32 = dist;

                Index_And_Dist_Pair pair = {
                    .index_a = i,
                    .index_b = j,
                    // .dist_sqr = dist,
                    .dist_sqr = dist_f32,
                };

                // Array_Append(&dists_and_pairs, pair);
                dists_and_pairs.items[pairs_count++] = pair;
            }
        }

        ASSERT(pairs_count == number_of_pairs);
    }


    // debug(junction_boxs.count);
    // debug(dists_and_pairs.count);

    // adds 70ms on input
    qsort(dists_and_pairs.items, dists_and_pairs.count, sizeof(dists_and_pairs.items[0]), compare_dists_and_pairs);


    Index_Type group_array [junction_boxs.count];
    Index_Type group_counts[junction_boxs.count];

    Mem_Zero(group_array,  sizeof(group_array));
    Mem_Zero(group_counts, sizeof(group_counts));

    Index_Type num_groups = 0;

    s64 part_1 = 0;
    s64 part_2 = 0;

    for (u64 i = 0; i < dists_and_pairs.count; i++) {
        if (i == num_connections_to_make) {

            Index_Type groups_count_copy[num_groups+1];
            Mem_Copy(groups_count_copy, group_counts, (num_groups+1)*sizeof(group_counts[0]));

            // grap the top 3 counts
            qsort(groups_count_copy, num_groups+1, sizeof(groups_count_copy[0]), compare_Index_Types);


            u64 total = 1;
            for (u32 i = 0; i < 3; i++) {
                Index_Type count = groups_count_copy[i];
                // ASSERT(count != 0);
                total *= count;
            }

            part_1 = total;
        }

        Index_And_Dist_Pair pair = dists_and_pairs.items[i];

        Index_Type index_a_group = group_array[pair.index_a];
        Index_Type index_b_group = group_array[pair.index_b];

        if (index_a_group == 0 && index_b_group == 0) {
            // neither of them are in a group
            num_groups += 1;
            group_array[pair.index_a] = num_groups;
            group_array[pair.index_b] = num_groups;
            group_counts[num_groups] = 2;

        } else if (index_a_group != 0 && index_b_group == 0) {
            // add b to group a
            group_array[pair.index_b] = index_a_group;
            group_counts[index_a_group] += 1;

        } else if (index_a_group == 0 && index_b_group != 0) {
            // add a to group b
            group_array[pair.index_a] = index_b_group;
            group_counts[index_b_group] += 1;

        } else if (index_a_group != 0 && index_b_group != 0) {
            // both of them are in a group, make them have the same group
            if (index_a_group == index_b_group) continue;

            // maybe choose the smaller one?
            Index_Type to_find         = index_b_group;
            Index_Type to_replace_with = index_a_group;
            for (u64 j = 0; j < junction_boxs.count; j++) {
                if (group_array[j] == to_find) {
                    group_array[j] = to_replace_with;
                }
            }
            group_counts[to_replace_with] += group_counts[to_find];
            group_counts[to_find] = 0;

        } else {
            UNREACHABLE();
        }


        if (group_counts[index_a_group] == junction_boxs.count || group_counts[index_b_group] == junction_boxs.count) {
            // all the boxs are connected
            s32 a_x = junction_boxs.items[pair.index_a].x;
            s32 b_x = junction_boxs.items[pair.index_b].x;
            part_2 = a_x * b_x;
            break;
        }
    }



    Solution solution = {
        .part_1 = part_1,
        .part_2 = part_2,
    };
    return solution;
}


#define DAY "08"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(40, 25272);

    Do_Input();

    Perf_Input(Get_Input(), 10);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

