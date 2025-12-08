
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


typedef struct {
    u32 index_a;
    u32 index_b;
    s64 dist_sqr;
} Index_And_Dist_Pair;
Make_Array(Index_And_Dist_Pair, Distance_Pairs_Array);

internal int compare_dists_and_pairs(const void *_a, const void *_b) {
    Index_And_Dist_Pair *a = (Index_And_Dist_Pair*)_a;
    Index_And_Dist_Pair *b = (Index_And_Dist_Pair*)_b;
    if      (a->dist_sqr < b->dist_sqr)     return -1;
    else if (a->dist_sqr > b->dist_sqr)     return  1;
    else                                    return  0;
}



internal int compare_u32s(const void *_a, const void *_b) {
    u32 a = *(u32*)_a;
    u32 b = *(u32*)_b;
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


    // takes about 6ms on input
    Distance_Pairs_Array dists_and_pairs = { .allocator = allocator };
    for (u64 i = 0; i < junction_boxs.count-1; i++) {
        Vector3 a = junction_boxs.items[i];
        for (u64 j = i+1; j < junction_boxs.count; j++) {
            Vector3 b = junction_boxs.items[j];

            s64 dist = dist_sqr(a, b);

            Index_And_Dist_Pair pair = {
                .index_a = i,
                .index_b = j,
                .dist_sqr = dist,
            };

            Array_Append(&dists_and_pairs, pair);
        }
    }

    // debug(junction_boxs.count);
    // debug(dists_and_pairs.count);

    // adds 70ms on input
    qsort(dists_and_pairs.items, dists_and_pairs.count, sizeof(dists_and_pairs.items[0]), compare_dists_and_pairs);


    u32 group_array [junction_boxs.count];
    u32 group_counts[junction_boxs.count];

    Mem_Zero(group_array,  sizeof(group_array));
    Mem_Zero(group_counts, sizeof(group_counts));

    u32 num_groups = 0;

    s64 part_1 = 0;
    s64 part_2 = 0;

    for (u64 i = 0; i < dists_and_pairs.count; i++) {
        if (i == num_connections_to_make) {

            u32 groups_count_copy[num_groups+1];
            Mem_Copy(groups_count_copy, group_counts, (num_groups+1)*sizeof(group_counts[0]));

            // grap the top 3 counts
            qsort(groups_count_copy, num_groups+1, sizeof(groups_count_copy[0]), compare_u32s);


            u64 total = 1;
            for (u32 i = 0; i < 3; i++) {
                u32 count = groups_count_copy[i];
                // ASSERT(count != 0);
                total *= count;
            }

            part_1 = total;
        }

        Index_And_Dist_Pair pair = dists_and_pairs.items[i];

        u32 index_a_group = group_array[pair.index_a];
        u32 index_b_group = group_array[pair.index_b];

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
            u32 to_find = index_b_group;
            u32 to_replace_with = index_a_group;
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

    // Perf_Input(Get_Input(), 100);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

