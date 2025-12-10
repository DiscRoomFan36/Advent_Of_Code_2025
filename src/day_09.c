
#include <stdio.h>

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
    for (u32 i = 0; i < positions.count; i++) {
        Vector2 point = positions.items[i];
        // if a point is in the middle of the rectangle were making...
        if (point_inside_rectangle(min_x, max_x, min_y, max_y, point.x, point.y)) {
            return false;
        }
    }

#if 0

    for (u32 i = 0; i < positions.count; i++) {
        Line line = get_line(positions, i);

        // if (line.start.x != line.end.x)

        // +1 & -1 to make the rectangle checking the slightly smaller one.
        bool rectanlges_intersect = (
            max_x-1 >= line.start.x &&      // r1 right edge past r2 left
            min_x+1 <= line.end.x   &&      // r1 left edge past r2 right
            max_y-1 >= line.start.y &&      // r1 top edge past r2 bottom
            min_y+1 <= line.end.x           // r1 bottom edge past r2 top
        );

        if (rectanlges_intersect) return false;
    }

#else

    for (u32 i = 0; i < positions.count; i++) {
        Line line = get_line(positions, i);

        // check if the mid point of the line is in the middle.
        //
        // this check is not exhaustive, as a malicious input could have
        // a mid point outside of the rectangle but still intersect.
        s32 mid_x = (line.start.x + line.end.x) / 2;
        s32 mid_y = (line.start.y + line.end.y) / 2;
        if (point_inside_rectangle(min_x, max_x, min_y, max_y, mid_x, mid_y)) {
            return false;
        }
    }


    // for (u32 i = 0; i < positions.count; i++) {
    //     Line line = get_line(positions, i);

    //     // check if some mid point is within the line. this check is also not exaustive.
    //     // but much more likely to stop some malicious input.
    //     for (u32 j = 1; j < 8; j++) {
    //         s32 mid_x = ((line.end.x - line.start.x) * j) / 8 + line.start.x;
    //         s32 mid_y = ((line.end.y - line.start.y) * j) / 8 + line.start.y;
    //         if (point_inside_rectangle(min_x, max_x, min_y, max_y, mid_x, mid_y)) {
    //             return false;
    //         }
    //     }

    // }
#endif

    return true;
}



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

    for (u64 i = 0; i < positions.count-1; i++) {
        Vector2 p1 = positions.items[i];
        // +2, the ones right next to each other are definently not the biggest.
        for (u64 j = i+2; j < positions.count; j++) {
            Vector2 p2 = positions.items[j];

            u64 rec_size = get_rec_size(p1, p2);
            if (max_rectangle_size < rec_size) max_rectangle_size = rec_size;
        }
    }


    // part 2
    u64 max_rectanlge_that_fits = 0;

    for (u64 i = 0; i < positions.count-1; i++) {
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

    // Perf_Input(Get_Input(), 100);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

