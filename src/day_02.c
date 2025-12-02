
#include <stdio.h>

#include "common.h"


internal Solution solve_input(String input) {

    for (u32 i = 0; i < input.length; i++) {
        if (input.data[i] == ',') input.data[i] = '\n';
    }

    s64 invalid_id_sum = 0;
    s64 extra_invalid_id_sum = 0;

    Int_Array digits = ZEROED;
    digits.allocator = Scratch_Get();

    String_Array lines = string_to_null_terminated_lines(input, true);
    for (u64 k = 0; k < lines.count; k++) {
        String line = lines.items[k];


        s64 start, end;
        ASSERT(sscanf(line.data, "%ld-%ld", &start, &end) == 2);


        for (u64 i = start; i <= end; i++) {
            digits.count = 0;

            u32 int_log_10 = 0;
            u64 n = i;
            while (n > 0) {
                u64 div = n / 10;
                u64 mod = n % 10;

                Array_Append(&digits, mod);
                n = div;
            }


            // part 1
            if (digits.count % 2 == 0) {
                bool flag = true;
                for (u32 j = 0; j < digits.count/2; j++) {
                    u32 l = digits.count/2+j;
                    if (digits.items[j] != digits.items[l]) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    // debug(i);
                    invalid_id_sum       += i;
                    extra_invalid_id_sum += i;
                    continue;
                }
            }


            // handle 3 or more.
            //
            // j == number of repeats
            for (u32 j = 3; j <= digits.count; j++) {
                // the patern wont fit into the array perfectly
                if (digits.count % j != 0) continue;

                u32 group_size = digits.count / j;

                bool flag = true;
                // check all the groups are the same.
                for (u32 l = 1; l < j; l++) {

                    for (u32 m = 0; m < group_size; m++) {
                        s64 n1 = digits.items[l     * group_size + m];
                        s64 n2 = digits.items[(l-1) * group_size + m];

                        if (n1 != n2) {
                            flag = false;
                            break;
                        }
                    }

                    if (!flag) break;
                }

                if (flag) {
                    extra_invalid_id_sum += i;
                    break; // dont check more sizes
                }
            }

        }
    }

    Solution solution = {
        .part_1 = invalid_id_sum,
        .part_2 = extra_invalid_id_sum,
    };
    return solution;
}


#define DAY "02"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(1227775554, 4174379265);

    Do_Input();

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

