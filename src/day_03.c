
#include <stdio.h>

#include "common.h"


Make_Array(u32, u32_Array);


internal s64 find_largest_jolts(u32_Array array, u32 depth) {
    s64 result = 0;
    while (depth != 0) {
        s64 max_num   = 0;
        u32 max_index = 0;
        for (u32 i = 0; i <= array.count - depth; i++) {
            s64 x = array.items[i];
            if (x == 9) {
                max_num   = x;
                max_index = i;
                break;
            }

            if (max_num < x) {
                max_num   = x;
                max_index = i;
            }
        }

        array.count -= max_index + 1;
        array.items += max_index + 1;
        result = result * 10 + max_num;
        depth -= 1;
    }

    return result;
}



internal Solution solve_input(String input) {
    s64 total_jolts       = 0;
    s64 total_super_jolts = 0;

    u32_Array digits = { .allocator = Scratch_Get() };

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        digits.count = 0;
        for (u64 i = 0; i < line.length; i++) {
            char c = line.data[i];
            Array_Append(&digits, c - '0');
        }

        total_jolts       += find_largest_jolts(digits,  2);
        total_super_jolts += find_largest_jolts(digits, 12);
    }

    Solution solution = {
        .part_1 = total_jolts,
        .part_2 = total_super_jolts,
    };
    return solution;
}


void do_statistics(Int_Array times) {
    u64 total_time_ns = 0;
    for (u64 i = 0; i < times.count; i++) {
        u64 time_ns = times.items[i];
        total_time_ns += time_ns;
    }
    u64 average_time_ns = total_time_ns / times.count;

    u64 sum_of_errors_ns = 0;
    for (u64 i = 0; i < times.count; i++) {
        s64 time_ns = times.items[i];
        sum_of_errors_ns += Abs((s64)average_time_ns - time_ns);
    }
    u64 average_error_ns = sum_of_errors_ns / times.count;

    f64 variance = (f64)average_error_ns / (f64)average_time_ns;

    printf("Statistics:\n");
    printf("    Iterations    : %7ld runs\n", times.count);
    printf("    Total Time    :  "); print_time_ns_in_parts(total_time_ns);    printf("\n");
    printf("    Average Time  :  "); print_time_ns_in_parts(average_time_ns);  printf("\n");
    printf("    Average Error : ±"); print_time_ns_in_parts(average_error_ns); printf("\n");
    printf("    Variance (%%)  : ±%5.2f%%\n", variance * 100);
}


#define Perf_Input(num_iterations)                                          \
    do {                                                                    \
        String input = Get_Input();                                         \
        Solution base_solution = solve_input(input);                        \
                                                                            \
        Int_Array times = ZEROED;                                           \
        for (s32 i = 0; i < (num_iterations); i++) {                        \
            start_timer();                                                  \
                Solution solution = solve_input(input);                     \
            u64 time = finish_timer();                                      \
            ASSERT(Mem_Eq(&base_solution, &solution, sizeof(solution)));    \
            Array_Append(&times, time);                                     \
        }                                                                   \
                                                                            \
        do_statistics(times);                                               \
    } while (0)



#define DAY "03"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(357, 3121910778619);

    Do_Input();

    Perf_Input(1000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

