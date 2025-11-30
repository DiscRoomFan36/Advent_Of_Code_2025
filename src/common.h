#pragma once



#define BESTED_IMPLEMENTATION
#include "../thirdparty/Bested.h"


global_variable Arena_Pool _pool = ZEROED;
global_variable Arena_Pool *pool = &_pool;


#define Scratch_Get()           Pool_Get(pool)
#define Scratch_Drop(arena)     Pool_Release(pool, (arena))
#define Scratch_Free()          Pool_Free_Arenas(pool)




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
        // PANIC("COULD NOT LOAD FILE %s", file_to_load);
    }
    return result;
}


#define Get_Example()   file_name_to_example_or_input(S(__FILE__), true)
#define Get_Input()     file_name_to_example_or_input(S(__FILE__), false)



global_variable u64 timer_start = 0;
internal void start_timer(void) {
    timer_start = nanoseconds_since_unspecified_epoch();
}

internal void finish_timer(void) {
    u64 timer_end = nanoseconds_since_unspecified_epoch();

    u64 total_time_ns = timer_end - timer_start;

    u64 ns = (total_time_ns           ) % 1000;
    u64 us = (total_time_ns / THOUSAND) % 1000;
    u64 ms = (total_time_ns / MILLION ) % 1000;
    u64  s = (total_time_ns / BILLION ) % 1000;

    printf("[[[[ ->    time: %4lds, %4ldms, %4ldus, %4ldns    <- ]]]]\n", s, ms, us, ns);
}



///////////////////////////////////////////////////
//                     Helpers
///////////////////////////////////////////////////



#define Make_Array(type, name)      \
    typedef struct {                \
        _Array_Header_;             \
        type *items;                \
    } name



Make_Array(s64, Int_Array);


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
    return a-b;
}

// smallest to largest
void sort_int_array(Int_Array *array) {
    qsort(array->items, array->count, sizeof(array->items[0]), compare_ints);
}






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



#define debug(x)                                            \
    do {                                                    \
        printf("DEBUG: %s = ", #x);                         \
        _Generic(x,                                         \
            s64: print_s64(&x),  u64: print_u64(&x),        \
            s32: print_s32(&x),  u32: print_u32(&x),        \
            s16: print_s16(&x),  u16: print_u16(&x),        \
            s8 : print_s8 (&x),  u8 : print_u8 (&x),        \
            f32: print_f32(&x),  f64: print_f64(&x),        \
            String: print_string(&x),                       \
            Int_Array: print_int_array(&x),                 \
            default: printf("?UNKNOWN_TYPE?")               \
        );                                                  \
        printf("\n");                                       \
    } while (0)







