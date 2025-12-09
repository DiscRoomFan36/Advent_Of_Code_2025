
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



internal Solution solve_input_1(String input) {
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





#define DIM_SIZE        32

typedef struct {
    Vector3 vec;
    Index_Type id; // index into junction_boxs array.
} Vector_And_Id;

typedef struct {
    Vector_And_Id items[8];
    u32 count;
} Vector3_Box;

// a 3d array that holds the junction boxs
global_variable Vector3_Box spacial_array[DIM_SIZE*DIM_SIZE*DIM_SIZE];

global_variable u32         spacial_array_step_size;


internal Vector3_Box *ijk_to_box(u32 i, u32 j, u32 k) {
    return &spacial_array[k*DIM_SIZE*DIM_SIZE + j*DIM_SIZE + i];
}


internal Vector3_Box *Vector3_to_box(Vector3 vec) {
    // NOTE spacial_array_step_size wiil allways be a power of 2
    // NOTE z array could be used here.
    u32 i = vec.x / spacial_array_step_size;
    u32 j = vec.y / spacial_array_step_size;
    u32 k = vec.z / spacial_array_step_size;
    return ijk_to_box(i, j, k);
}

internal Solution solve_input_2(String input) {
    Arena *allocator = Scratch_Get();
    Vector3_Array junction_boxs = { .allocator = allocator };

    String_Array lines = string_split_by(input, "\n");

    s32 max_dim = 0;

    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Vector3 box;
        ASSERT(sscanf(line.data, "%d,%d,%d", &box.x, &box.y, &box.z) == 3);

        if (max_dim < box.x) max_dim = box.x;
        if (max_dim < box.y) max_dim = box.y;
        if (max_dim < box.z) max_dim = box.z;

        Array_Append(&junction_boxs, box);
    }

    // u32 num_connections_to_make_for_part_1 = junction_boxs.count == 20 ? 10 : 1000;

    // index type should be able to index every junction box.
    ASSERT( junction_boxs.count < (1UL << (sizeof(Index_Type)*8-1)) );


    debug(max_dim);

    // plus 1 to go past.
    u32 max_log_2 = int_log_2(max_dim)+1;
    u32 new_max_dim = int_pow(2, max_log_2);
    debug(max_log_2);
    debug(new_max_dim);


    Mem_Zero(spacial_array, sizeof(spacial_array));
    spacial_array_step_size = new_max_dim / DIM_SIZE;

    for (u32 i = 0; i < junction_boxs.count; i++) {
        Vector3 vec = junction_boxs.items[i];

        Vector_And_Id v_a_i = {vec, i};

        Vector3_Box *box = Vector3_to_box(vec);
        ASSERT(box->count < Array_Len(box->items));
        box->items[box->count++] = v_a_i;
    }





    Solution solution = {
        .part_1 = 0,
        .part_2 = 0,
    };
    return solution;
}







internal Solution solve_input(String input) {
    Solution sol_1 = solve_input_1(input);
    Solution sol_2 = solve_input_2(input);

    printf("part_1 %s, (%12ld, %12ld)\n", sol_1.part_1 == sol_2.part_1 ? " True" : "False", sol_1.part_1, sol_2.part_1);
    printf("part_2 %s, (%12ld, %12ld)\n", sol_1.part_2 == sol_2.part_2 ? " True" : "False", sol_1.part_2, sol_2.part_2);

    return sol_1;
}



#define DAY "08"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(40, 25272);

    Do_Input();

    // Perf_Input(Get_Input(), 10);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

