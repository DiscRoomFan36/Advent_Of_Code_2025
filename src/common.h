#pragma once



// make these defaults different for this project.
//
// A bigger Arena and array will reduce the
// number of times it has to reallocate.

#define ARENA_REGION_DEFAULT_CAPACITY   (32 * MEGABYTE)
#define ARRAY_INITAL_CAPACITY           512



#define BESTED_IMPLEMENTATION
#include "../thirdparty/Bested.h"


global_variable Arena_Pool _pool = ZEROED;
global_variable Arena_Pool *pool = &_pool;


#define Scratch_Get()           Pool_Get(pool)
#define Scratch_Drop(arena)     Pool_Release(pool, (arena))
#define Scratch_Free()          Pool_Free_Arenas(pool)


////////////////////////////////////////////
//           Array Helpers
////////////////////////////////////////////

#define Make_Array(type, name)      \
    typedef struct {                \
        _Array_Header_;             \
        type *items;                \
    } name

// Common Arrays
Make_Array(s64, Int_Array);
Make_Array(String, String_Array);

// Other Ints
typedef Int_Array s64_Array;
Make_Array(u64, u64_Array);

Make_Array(s32, s32_Array);
Make_Array(u32, u32_Array);

Make_Array(s16, s16_Array);
Make_Array(u16, u16_Array);

Make_Array(s8,  s8_Array);
Make_Array(u8,  u8_Array);






#define MAX_TEMP_FILE_SIZE      (1 * MEGABYTE)
// overwrites temeratry buffer every call.
internal String temp_read_entire_file(const char *filename) {
    local_persist u8 temp_file_storeage[MAX_TEMP_FILE_SIZE];

    FILE *file = fopen(filename, "rb");
    String result = ZEROED;
    result.data = (void*) temp_file_storeage;

    if (file) {
        fseek(file, 0, SEEK_END);
        s64 size = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (size >= (s64)MAX_TEMP_FILE_SIZE-1) {
            PANIC("FILE IS TO BIG, WAS %.02fMB", (f64)size / (f64)MEGABYTE);
        }

        if (size >= 0) {
            result.length = size;
            fread(result.data, 1, result.length, file);
            result.data[result.length] = 0; // zero terminate for fun.
        }
        fclose(file);
    }

    return result;
}




#define INPUT_FOLDER "./input/"

internal String file_name_to_example_or_input(String filename, bool get_example) {
    // printf("%s\n", filename);

    String day_name = filename;

    while (true) {
        s64 index = String_Find_Index_Of_Char(day_name, '/');
        if (index == -1) break;
        String_Advance(&day_name, index + 1);
    }

    s64 index_of_dot = String_Find_Index_Of_Char(day_name, '.');
    ASSERT(index_of_dot != -1);
    day_name.length = index_of_dot;


    s64 index_of_underscore = String_Find_Index_Of_Char(day_name, '_');
    ASSERT(index_of_underscore != -1);
    String day_numbers = String_Advanced(day_name, index_of_underscore+1);

    const char *file_to_load;
    if (get_example) {
        file_to_load = temp_sprintf(INPUT_FOLDER""S_Fmt"_example.txt", S_Arg(day_numbers));
    } else {
        file_to_load = temp_sprintf(INPUT_FOLDER""S_Fmt"_input.txt", S_Arg(day_numbers));
    }

    String result = temp_read_entire_file(file_to_load);
    if (result.length == 0) {
        PANIC("COULD NOT LOAD FILE %s", file_to_load);
    }
    return result;
}


#define Get_Example()   file_name_to_example_or_input(S(__FILE__), true)
#define Get_Input()     file_name_to_example_or_input(S(__FILE__), false)



global_variable u64 timer_start = 0;
internal void start_timer(void) {
    timer_start = nanoseconds_since_unspecified_epoch();
}

// returns total time in nano_seconds
internal u64 finish_timer(void) {
    u64 timer_end = nanoseconds_since_unspecified_epoch();

    u64 total_time_ns = timer_end - timer_start;
    return total_time_ns;
}


typedef struct {
    u64 ns;
    u64 us;
    u64 ms;
    u64  s;

    f64 in_seconds;
} Time_In_Parts;

internal Time_In_Parts time_in_ns_to_time_in_parts(u64 time_in_ns) {
    Time_In_Parts result = {
        .ns = (time_in_ns           ) % 1000,
        .us = (time_in_ns / THOUSAND) % 1000,
        .ms = (time_in_ns / MILLION ) % 1000,
        . s = (time_in_ns / BILLION ) % 1000,

        // this might result in the loss of some accuracy,
        // but as long as the time is small it should be ok.
        .in_seconds = (f64)time_in_ns / NANOSECONDS_PER_SECOND,
    };
    return result;
}


#define print_time_in_parts(time)   printf("%4lds, %4ldms, %4ldus, %4ldns", (time).s, (time).ms, (time).us, (time).ns)

#define print_time_ns_in_parts(time_ns)                                 \
    do {                                                                \
        Time_In_Parts time = time_in_ns_to_time_in_parts(time_ns);      \
        print_time_in_parts(time);                                      \
    } while (0)


internal void print_time(u64 time_in_ns) {
    Time_In_Parts time = time_in_ns_to_time_in_parts(time_in_ns);
    printf("[[[[ ->    time: ");
    print_time_in_parts(time);
    printf("    <- ]]]]\n");
}


void do_statistics(u64_Array times) {
    u64 total_time_ns = 0;
    for (u64 i = 0; i < times.count; i++) {
        u64 time_ns = times.items[i];
        total_time_ns += time_ns;
    }
    u64 average_time_ns = total_time_ns / times.count;

    u64 sum_of_errors_ns = 0;
    for (u64 i = 0; i < times.count; i++) {
        u64 time_ns = times.items[i];
        sum_of_errors_ns += Abs((s64)average_time_ns - (s64)time_ns);
    }
    u64 average_error_ns = sum_of_errors_ns / times.count;

    f64 variance = (f64)average_error_ns / (f64)average_time_ns;

    printf("Statistics:\n");
    printf("    Iterations    : %7ld runs\n", times.count);
    printf("    Total Time    :  "); print_time_ns_in_parts(total_time_ns);    printf("\n");
    printf("    Average Time  :  "); print_time_ns_in_parts(average_time_ns);  printf("\n");
    printf("    Std. Error    : ±"); print_time_ns_in_parts(average_error_ns); printf("\n");
    printf("    Variance (%%)  : ±%5.2f%%\n", variance * 100);
}



typedef struct {
    s64 part_1;
    s64 part_2;
} Solution;



#define Do_Example(part_1_solution, part_2_solution)                                                    \
    do {                                                                                                \
        String input_file = Get_Example();                                                              \
        start_timer();                                                                                  \
            Solution example = solve_input(input_file);                                                 \
        u64 time = finish_timer();                                                                      \
                                                                                                        \
        printf("    example:\n");                                                                       \
        printf("        part 1: |%18ld|,   correct: |%18ld|\n", example.part_1, (s64)part_1_solution);  \
        printf("        part 2: |%18ld|,   correct: |%18ld|\n", example.part_2, (s64)part_2_solution);  \
                                                                                                        \
        print_time(time);                                                                               \
    } while (0)


#define Do_Input()                                              \
    do {                                                        \
        String input_file = Get_Input();                        \
        start_timer();                                          \
            Solution input = solve_input(input_file);           \
        u64 time = finish_timer();                              \
                                                                \
        printf("    input  :\n");                               \
        printf("        part 1: |%18ld|\n", input.part_1);      \
        printf("        part 2: |%18ld|\n", input.part_2);      \
                                                                \
        print_time(time);                                       \
    } while (0)



#define Perf_Input(input, num_iterations)                                   \
    do {                                                                    \
        String _input = (input);                                            \
        Solution base_solution = solve_input(_input);                       \
                                                                            \
        u64_Array times = ZEROED;                                           \
        for (s32 i = 0; i < (num_iterations); i++) {                        \
            start_timer();                                                  \
                Solution solution = solve_input(_input);                    \
            u64 time = finish_timer();                                      \
            ASSERT(Mem_Eq(&base_solution, &solution, sizeof(solution)));    \
            Array_Append(&times, time);                                     \
        }                                                                   \
                                                                            \
        do_statistics(times);                                               \
    } while (0)



///////////////////////////////////////////////////
//                Other Helpers
///////////////////////////////////////////////////

#define Proper_Mod(x, y) ((((x) % (y)) + (y)) % (y))



#define Array_Swap(array, i, j)                                 \
    do {                                                        \
        Typeof((array)->items[0]) tmp = (array)->items[(i)];    \
        (array)->items[(i)] = (array)->items[(j)];              \
        (array)->items[(j)] = tmp;                              \
    } while (0)


#define Array_Reverse(array)                            \
    do {                                                \
        for (u64 i = 0; i < (array)->count/2; i++) {    \
            u64 j = ((array)->count-1)-i;               \
            Array_Swap(array, i, j);                    \
        }                                               \
    } while (0)

#define Array_Eq(array, i, j) ((array).items[(i)] == (array).items[(j)])




#define static_array_to_array(array_type, xs) { .items = xs, .count = Array_Len(xs), .capacity = Array_Len(xs), .allocator = Scratch_Get() }


// modifies the input string
String_Array string_to_null_terminated_lines(String input, bool skip_empty) {
    String_Array result = ZEROED;
    result.allocator = Scratch_Get();

    String_Get_Next_Line_Flag flag = SGNL_Trim;
    if (skip_empty) flag |= SGNL_Skip_Empty;

    while (true) {
        String line = String_Get_Next_Line(&input, NULL, flag);
        if (line.length == 0) break;

        line.data[line.length] = 0;
        Array_Append(&result, line);
    }

    return result;
}



typedef struct {
    Arena *allocator;
    bool dont_trim_lines;
    bool keep_empty_last_line;
} string_split_by_Opt;

#define string_split_by(input, split_by, ...) _string_split_by((input), split_by, (string_split_by_Opt){ __VA_ARGS__ })

String_Array _string_split_by(String input, const char *split_by, string_split_by_Opt opt) {
    String needle = S(split_by);

    String_Array result = ZEROED;
    if (opt.allocator)  result.allocator = opt.allocator;
    else                result.allocator = Scratch_Get();

    while (true) {
        s64 index = String_Find_Index_Of(input, needle);
        if (index == -1) {
            Array_Append(&result, input);
            break;
        }

        String advanced = String_Advanced(input, index + needle.length);
        input.length = index;
        Array_Append(&result, input);
        input = advanced;
    }

    if (result.items[result.count-1].length == 0) {
        if (!opt.keep_empty_last_line) {
            result.count -= 1;
        }
    }

    // null terminate all strings.
    for (u64 i = 0; i < result.count; i++) {
        String line = result.items[i];
        if (!opt.dont_trim_lines) line = String_Trim_Right(line);

        result.items[i] = String_Duplicate(result.allocator, line, .null_terminate = true);
    }

    return result;
}




s64 index_of(Int_Array array, s64 to_find) {
    for (u64 i = 0; i < array.count; i++) {
        if (array.items[i] == to_find) {
            return i;
        }
    }
    return -1;
}


int compare_ints(const void *_a, const void *_b) {
    s64 a = *(s64*)_a;
    s64 b = *(s64*)_b;
    if      (a < b)  return -1;
    else if (a > b)  return  1;
    else             return  0;
}

// smallest to largest
void sort_int_array(Int_Array *array) {
    qsort(array->items, array->count, sizeof(array->items[0]), compare_ints);
}

void debug_print_around(Int_Array array, s64 low, s64 high) {
    low = Max(low, 0);
    high = Min(high, (s64)array.count-1);
    printf("DEBUG around:\n");
    for (s64 i = low; i <= high; i++) {
        printf("    %ld: %ld\n", i, array.items[i]);
    }
}


u64 binary_search(Int_Array array, s64 x) {
    s64 low = 0;
    s64 high = array.count-1;
    while (low <= high) {
        s64 mid = low + (high - low) / 2;

        // Check if x is present at mid
        // if (all_invalid_ids.items[mid] == range.start) {
        //     break;
        // }

        if (array.items[mid] < x) {
            // If x greater, ignore left half
            low = mid + 1;

        } else {
            // If x is smaller, ignore right half
            high = mid - 1;
        }
    }
    return low;
}





u32 int_log_10(u64 x) {
    u32 result = 0;
    while (x > 0) { result += 1; x /= 10; }
    return result;
}

u32 int_log_2(u64 x) {
    u32 result = 0;
    while (x > 0) { result += 1; x /=  2; }
    return result;
}

u32 int_log_n(u64 x, u64 n) {
    // these are much faster because constant division is faster.
    if (n == 2)  return int_log_2 (x);
    if (n == 10) return int_log_10(x);

    u32 result = 0;
    while (x > 0) { result += 1; x /=  n; }
    return result;
}



// dose the fast thing.
s64 int_pow(s64 x, u64 y) {
    if (x == 0) return 0;

    s64 result = 1;
    s64 keep_squaring = x;
    while (y != 0) {
        if (y & 1) result *= keep_squaring;
        y = y >> 1;
        keep_squaring = keep_squaring*keep_squaring;
    }

    return result;
}

f64 f64_int_pow(f64 x, s64 y) {
    if (x == 0) return 0;

    if (y < 0) {
        x = 1 / x;
        y = -y;
    }

    f64 result = 1;
    f64 keep_squaring = x;
    while (y != 0) {
        if (y & 1) result *= keep_squaring;
        y = y >> 1;
        keep_squaring = keep_squaring*keep_squaring;
    }

    return result;
}




////////////////////////////////////////////////////////////////
//                       Debug print.
////////////////////////////////////////////////////////////////

// TODO put in bested.h


void print_s64   (void *_x) { s64 x    = *(s64*)   _x; printf("%ld", x); }
void print_u64   (void *_x) { u64 x    = *(u64*)   _x; printf("%ld", x); }

void print_s32   (void *_x) { s32 x    = *(s32*)   _x; printf("%d",  x); }
void print_u32   (void *_x) { u32 x    = *(u32*)   _x; printf("%d",  x); }

void print_s16   (void *_x) { s16 x    = *(s16*)   _x; printf("%d",  x); }
void print_u16   (void *_x) { u16 x    = *(u16*)   _x; printf("%d",  x); }

void print_s8    (void *_x) { s8  x    = *(s8 *)   _x; printf("%d",  x); }
void print_u8    (void *_x) { u8  x    = *(u8 *)   _x; printf("%d",  x); }

void print_f32   (void *_x) { f32 x    = *(f32*)   _x; printf("%f",  x); }
void print_f64   (void *_x) { f64 x    = *(f64*)   _x; printf("%f",  x); }


void print_string(void *_x) { String x = *(String*)_x; printf("\""S_Fmt"\"", S_Arg(x)); }


void print_int_array(void *_array) {
    Int_Array array = *(Int_Array*)_array;
    printf("{\n    ");
    for (u64 i = 0; i < array.count; i++) {
        if (i != 0 && i % 10 == 0) printf("\n    ");
        printf("%6ld, ", array.items[i]);
    }
    printf("\n}");
}

void print_string_array(void *_array) {
    String_Array array = *(String_Array*)_array;
    printf("{\n");
    for (u64 i = 0; i < array.count; i++) {
        printf("    ");
        print_string(&array.items[i]);
        printf(",\n");
    }
    printf("}");
}


#define generic_print(x)                                    \
    _Generic(x,                                             \
        s64: print_s64(&x),  u64: print_u64(&x),            \
        s32: print_s32(&x),  u32: print_u32(&x),            \
        s16: print_s16(&x),  u16: print_u16(&x),            \
        s8 : print_s8 (&x),  u8 : print_u8 (&x),            \
        f32: print_f32(&x),  f64: print_f64(&x),            \
        String: print_string(&x),                           \
        Int_Array: print_int_array(&x),                     \
        String_Array: print_string_array(&x),               \
        default: printf("?UNKNOWN_TYPE?")                   \
    )


#define debug(x)  do { printf("DEBUG: %s = ", #x); generic_print(x); printf("\n"); } while (0)


#define debug_break() asm("int3")





