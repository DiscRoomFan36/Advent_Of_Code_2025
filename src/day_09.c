
#include <stdio.h>

#include <errno.h>
#include <pthread.h>

#include "common.h"


typedef struct {
    s32 x, y;
} Vector2;
Make_Array(Vector2, Vector2_Array);


internal u64 get_rec_size(Vector2 p1, Vector2 p2) {
    u64 rec_width  = Abs(p1.x - p2.x) + 1;
    u64 rec_height = Abs(p1.y - p2.y) + 1;

    u64 rec_size = rec_width * rec_height;
    return rec_size;
}

bool Vector2_Eq(Vector2 p1, Vector2 p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}



typedef struct {
    Vector2 start, end;
} Line;

// #assert start.x <= end.x && start.y <= end.y
internal Line get_line(Vector2_Array positions, u32 index) {
    Vector2 p1 = positions.items[index];
    Vector2 p2 = positions.items[(index+1) % positions.count];

    Line result = {
        .start = {
            .x = Min(p1.x, p2.x),
            .y = Min(p1.y, p2.y),
        },
        .end = {
            .x = Max(p1.x, p2.x),
            .y = Max(p1.y, p2.y),
        },
    };
    return result;
}



internal bool point_inside_rectangle(s32 min_x, s32 max_x, s32 min_y, s32 max_y, s32 x, s32 y) {
    return ((min_x < x) && (x < max_x)) &&
           ((min_y < y) && (y < max_y));
}

internal bool rectangle_fully_enclosed(Vector2_Array positions, Vector2 p1, Vector2 p2) {
    s32 min_x = Min(p1.x, p2.x);
    s32 max_x = Max(p1.x, p2.x);
    s32 min_y = Min(p1.y, p2.y);
    s32 max_y = Max(p1.y, p2.y);

    // check if there is a point inside of this
    //
    // the line intersection check dose the same thing,
    // but doing this check first makes the whole thing 2x faster.
    for (u32 i = 0; i < positions.count; i++) {
        Vector2 point = positions.items[i];
        // if a point is in the middle of the rectangle were making...
        if (point_inside_rectangle(min_x, max_x, min_y, max_y, point.x, point.y)) {
            return false;
        }
    }

    // check if any line intersects with the rectangle
    for (u32 i = 0; i < positions.count; i++) {
        Line line = get_line(positions, i);

        // check if the rectanlge is intersecting.
        // bool intersect = !(line.end.y <= min_y || line.start.y >= max_y) && !(line.end.x <= min_x || line.start.x >= max_x);
        bool intersect = (line.end.x > min_x && line.start.x < max_x) &&
                         (line.end.y > min_y && line.start.y < max_y);

        if (intersect) return false;
    }

    return true;
}




#define USE_MULTITHREADING      true

#if USE_MULTITHREADING

internal u64 largest_rectangle_that_fits_with_starting_position(Vector2_Array positions, u32 i) {
    u64 max_rectanlge_that_fits = 0;

    Vector2 p1 = positions.items[i];

    for (u64 j = i+2; j < positions.count; j++) {
        Vector2 p2 = positions.items[j];

        u64 rec_size = get_rec_size(p1, p2);
        if (max_rectanlge_that_fits < rec_size) {
            if (rectangle_fully_enclosed(positions, p1, p2)) {
                max_rectanlge_that_fits = rec_size;
            }
        }
    }

    return max_rectanlge_that_fits;
}


Vector2_Array global_positions = ZEROED;
Atomic(u32)   atomic_counter = 0;
u64 result_array_for_threads[512];

internal void *thread_grab_from_stack_until_done(void *arg) {
    (void) arg;

    ASSERT(global_positions.count <= Array_Len(result_array_for_threads));

    while (true) {
        u32 i = Atomic_Add(&atomic_counter, 1);
        if (i >= global_positions.count) break;

        u64 result = largest_rectangle_that_fits_with_starting_position(global_positions, i);
        result_array_for_threads[i] = result;
    }

    return NULL;
}

#endif // USE_MULTITHREADING



internal Solution solve_input(String input) {
    Arena *allocator = Scratch_Get();
    Vector2_Array positions = { .allocator = allocator };

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Vector2 pos;
        ASSERT(sscanf(line.data, "%d,%d", &pos.x, &pos.y) == 2);
        Array_Append(&positions, pos);
    }


    // part 1
    u64 max_rectangle_size = 0;

    for (u64 i = 0; i < positions.count-2; i++) {
        Vector2 p1 = positions.items[i];
        // +2, the ones right next to each other are definently not the biggest.
        for (u64 j = i+2; j < positions.count; j++) {
            Vector2 p2 = positions.items[j];

            u64 rec_size = get_rec_size(p1, p2);
            if (max_rectangle_size < rec_size) max_rectangle_size = rec_size;
        }
    }


    // part 2

#if USE_MULTITHREADING


    global_positions = positions;
    atomic_counter = 0;

    // my machine has 14 CPU's
    #define NUM_TOTAL_THREADS 10
    pthread_t ids[NUM_TOTAL_THREADS-1] = ZEROED;
    for (u32 i = 0; i < Array_Len(ids); i++) {
        s32 res = pthread_create(&ids[i], NULL, thread_grab_from_stack_until_done, NULL);
        if (res != 0) {
            errno = res;
            perror("could not create thread");
            exit(1);
        }
    }

    thread_grab_from_stack_until_done(NULL);

    for (u32 i = 0; i < Array_Len(ids); i++) {
        s32 res = pthread_join(ids[i], NULL);
        if (res != 0) {
            errno = res;
            perror("could not join thread");
            exit(1);
        }
    }

    u64 max = 0;
    for (u32 i = 0; i < positions.count-2; i++) {
        if (max < result_array_for_threads[i]) max = result_array_for_threads[i];
    }
    // debug(max);
    u64 part_2 = max;

#else

    u64 max_rectanlge_that_fits = 0;

    for (u64 i = 0; i < positions.count-2; i++) {
        Vector2 p1 = positions.items[i];
        // +2, the ones right next to each other are definently not the biggest.
        for (u64 j = i+2; j < positions.count; j++) {
            Vector2 p2 = positions.items[j];

            u64 rec_size = get_rec_size(p1, p2);
            if (max_rectanlge_that_fits < rec_size) {
                if (rectangle_fully_enclosed(positions, p1, p2)) {
                    max_rectanlge_that_fits = rec_size;
                }
            }
        }
    }

    u64 part_2 = max_rectanlge_that_fits;

#endif


    Solution solution = {
        .part_1 = max_rectangle_size,
        .part_2 = part_2,
    };
    return solution;
}


#define DAY "09"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(50, 24);

    Do_Input();

    // Perf_Input(Get_Input(), 1000);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

