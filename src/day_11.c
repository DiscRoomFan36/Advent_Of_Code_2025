
#include <stdio.h>

#include "common.h"


typedef struct {
    char character;
    s64 number;
} Input_Line;
Make_Array(Input_Line, Input_Line_Array);


typedef struct {
    char l[3];
} Label;


#define MAX_LABELS      600 // kind of a hack, my input is 584 long.

global_variable Label labels_to_indexs[MAX_LABELS] = ZEROED;
global_variable u32 total_labels = 0;


internal u32 label_to_id(Label label) {
    for (u32 i = 0; i < total_labels; i++) {
        Label *checking = &labels_to_indexs[i];

        if (Mem_Eq(checking, &label, sizeof(label))) {
            // found it.
            return i;
        }
    }

    ASSERT(total_labels < Array_Len(labels_to_indexs));

    // its a new label
    Mem_Copy(&labels_to_indexs[total_labels], &label, sizeof(label));
    return total_labels++;
}


internal u32 chars_to_id(const char *chars) {
    Label label;
    String s = S(chars);
    ASSERT(s.length == 3);
    Mem_Copy(label.l, s.data, sizeof(char)*3);
    return label_to_id(label);
}




global_variable s32 memo_array[MAX_LABELS];
global_variable u16 in_to_out_ids[MAX_LABELS][MAX_LABELS] = ZEROED;


internal u64 id_to_out_count(u16 id) {
    if (memo_array[id] != -1) return memo_array[id];

    u32 total = 0;
    for (u64 i = 0; i < total_labels; i++) {
        if (in_to_out_ids[id][i]) {
            total += id_to_out_count(i);
        }
    }

    memo_array[id] = total;
    return total;
}

internal u64 number_of_ways_to_get_from_in_to_out(u16 id_in, u16 id_out) {
    Mem_Set(memo_array, -1, total_labels * sizeof(memo_array[0]));
    memo_array[id_out] = 1;

    return id_to_out_count(id_in);
}


internal Solution solve_input(String input) {

    Mem_Zero(in_to_out_ids, sizeof(in_to_out_ids));

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Label start;
        Mem_Copy(start.l, line.data, 3*sizeof(char));

        String_Advance(&line, 3 + 2);
        ASSERT(line.data[0] != ' ');
        u32 start_id = label_to_id(start);

        String_Array outputs = string_split_by(line, " ");

        for (u32 i = 0; i < outputs.count; i++) {
            Label label;
            Mem_Copy(label.l, outputs.items[i].data, 3*sizeof(char));

            u32 label_id = label_to_id(label);
            in_to_out_ids[start_id][label_id] = 1;
        }
    }

    s64 part_1 = number_of_ways_to_get_from_in_to_out(chars_to_id("you"), chars_to_id("out"));

    // fft always comes before dac
    u64 svr_to_fft = number_of_ways_to_get_from_in_to_out(chars_to_id("svr"), chars_to_id("fft"));
    u64 fft_to_dac = number_of_ways_to_get_from_in_to_out(chars_to_id("fft"), chars_to_id("dac"));
    u64 dac_to_out = number_of_ways_to_get_from_in_to_out(chars_to_id("dac"), chars_to_id("out"));


    u64 part_2 = svr_to_fft * fft_to_dac * dac_to_out;

    Solution solution = {
        .part_1 = part_1,
        .part_2 = part_2,
    };
    return solution;
}


#define DAY "11"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(5, 2);

    Do_Input();

    Perf_Input(Get_Input(), 1);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

