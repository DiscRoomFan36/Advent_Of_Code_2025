
#include <stdio.h>

#include "common.h"


typedef struct {
    u32 number_of_bits;

    u16 indicator_bits;
    u16_Array buttons;
    Int_Array voltages;
} Machine;
Make_Array(Machine, Machine_Array);


#define popcount(x)    __builtin_popcount(x)


#define MAX_NUMBER_OF_BITS 11

typedef struct {
    u8 bits[MAX_NUMBER_OF_BITS];
    u8 bit_count; // this is the number of bits that are 1
} Button_Bit_Array;

int compare_Button_Bit_Array(const void *_a, const void *_b) {
    Button_Bit_Array a = *(Button_Bit_Array*)_a;
    Button_Bit_Array b = *(Button_Bit_Array*)_b;
    if      (a.bit_count > b.bit_count)  return -1;
    else if (a.bit_count < b.bit_count)  return  1;
    else                                 return  0;
}


#define MAX_NUMBER_OF_BUTTONS 16
typedef struct {
    u32 buttons_count;
    Button_Bit_Array b[MAX_NUMBER_OF_BUTTONS];
} All_Buttons;


typedef struct {
    u16 c[MAX_NUMBER_OF_BITS];
} Counts;

// greedy.
internal s32 find_minimum_number_of_button_presses(Counts *final_counts, All_Buttons *buttons, Counts *counts, u32 depth, u32 start_i) {
    for (u32 i = start_i; i < buttons->buttons_count; i++) {
        // add the button[i] to count;

        // finds the maximum number of times this button can be applied before overflowing.
        u32 max_times = 99999;
        for (u32 j = 0; j < Array_Len(buttons->b[i].bits); j++) {
            if (buttons->b[i].bits[j]) {
                u32 number_left = (final_counts->c[j] - counts->c[j]);
                if (max_times > number_left) max_times = number_left;
            }
        }
        ASSERT(max_times != 99999);


        for (s32 k = max_times; k >= 0; k--) {
            Counts new_counts;
            for (u32 j = 0; j < Array_Len(buttons->b[i].bits); j++) {
                new_counts.c[j] = counts->c[j] + (k * buttons->b[i].bits[j]);
            }

            s32 result = find_minimum_number_of_button_presses(final_counts, buttons, &new_counts, depth+k, i+1);
            if (result != -1) return result;
        }




        // Counts new_counts = *counts;
        // for (u32 j = 0; j < Array_Len(buttons->b[i].bits); j++) {
        //     new_counts.c[j] += buttons->b[i].bits[j];
        // }

        // // check if it overdflowed.
        // bool overflowed = false;
        // bool reached_final_counts = true;
        // for (u32 j = 0; j < Array_Len(buttons->b[i].bits); j++) {
        //     if (new_counts.c[j] > final_counts->c[j]) {
        //         overflowed = true;
        //         break;
        //     }
        //     if (new_counts.c[j] != final_counts->c[j]) {
        //         reached_final_counts = false;
        //     }
        // }
        // if (overflowed) continue;

        // if (reached_final_counts) return depth+1;

        // s32 result = find_minimum_number_of_button_presses(final_counts, buttons, &new_counts, depth+1, i);
        // if (result != -1) return result;
    }

    if (Mem_Eq(final_counts, counts, sizeof(*counts))) return depth;

    return -1;
}



internal Solution solve_input(String input) {
    Arena *allocator = Scratch_Get();
    Machine_Array machines = { .allocator = allocator };

    // TODO("allocators");

    // debug_break();

    String_Array lines = string_split_by(input, "\n");
    for (u64 line_index = 0; line_index < lines.count; line_index++) {
        String line = lines.items[line_index];

        Machine machine = ZEROED;


        String_Array parts = string_split_by(line, " ");
        u32 number_of_lights = parts.items[0].length-2;
        machine.number_of_bits = number_of_lights;
        ASSERT(machine.number_of_bits < MAX_NUMBER_OF_BITS);

        // ASSERT(parts.count > 4); // this is just some minimum
        { // indicator lights
            String indicator_lights = parts.items[0];
            ASSERT(indicator_lights.data[0] == '[');
            ASSERT(indicator_lights.data[indicator_lights.length-1] == ']');


            for (u32 i = 1; i < indicator_lights.length-1; i++) {
                char c = indicator_lights.data[i];
                u32 bit_index = i-1;
                if (c == '#') {
                    machine.indicator_bits |= (1 << bit_index);
                }
            }
        }

        // buttons
        for (u32 i = 1; i < parts.count-1; i++) {
            u16 button = 0;
            // parse a button
            String button_list = parts.items[i];
            button_list.data   += 1;
            button_list.length -= 2;

            String_Array numbers = string_split_by(button_list, ",");

            for (u32 j = 0; j < numbers.count; j++) {
                String number = numbers.items[j];
                s32 num = atoi(number.data);
                // must be between 0 and 16
                ASSERT(Is_Between(num, 0, (s32)number_of_lights-1));
                button |= 1 << num;
                ASSERT(num < 10);
            }
            Array_Append(&machine.buttons, button);
        }

        { // voltages
            String volts = parts.items[parts.count-1];
            volts.data += 1;
            volts.length -= 2;

            String_Array volt_list = string_split_by(volts, ",");
            for (u32 i = 0; i < volt_list.count; i++) {
                String volt = volt_list.items[i];
                s32 num = atoi(volt.data);
                Array_Append(&machine.voltages, num);
            }
            ASSERT(machine.voltages.count == number_of_lights);
        }

        Array_Append(&machines, machine);
    }



    // debug_break();

    // part 1
    s64 total_number_of_presses = 0;
    for (u64 machine_index = 0; machine_index < machines.count; machine_index++) {
        Machine machine = machines.items[machine_index];

        u32 max_number_of_presses = 999;

        for (u32 i = 1; i < (1 << machine.buttons.count); i++) {
            u16 lights_state = 0;

            u32 number_of_bits_set = popcount(i);
            if (number_of_bits_set >= max_number_of_presses) continue;

            for (u32 j = 0; j < machine.buttons.count; j++) {
                u16 button = machine.buttons.items[j];
                if (i & (1 << j)) {
                    // push this button, toggle all set.
                    lights_state ^= button;
                }
            }


            if (lights_state == machine.indicator_bits) {
                // was correct
                max_number_of_presses = number_of_bits_set;
            }
        }


        ASSERT(max_number_of_presses != 999);
        total_number_of_presses += max_number_of_presses;
    }

    // part 2
    u32 min_number_of_presses_for_volts = 0;
    for (u64 machine_index = 0; machine_index < machines.count; machine_index++) {
        printf("%4ld/%4ld,    %f%%\n", machine_index, machines.count, ((f64)machine_index / machines.count) * 100);
        fflush(stdout);

        Machine machine = machines.items[machine_index];

        // these are the numbers were working with.
        // {183,25,183,38,44,185,38}

        // this is enough
        // u16 numbers[10] = ZEROED;
        u32 numbers_count = machine.number_of_bits;

        // u64 projuct_of_jolts = 1;
        // for (u32 i = 0; i < machine.voltages.count; i++) {
        //     projuct_of_jolts *= machine.voltages.items[i];
        // }
        // debug(projuct_of_jolts);

        // do a greedy recursive algorithum.

        ASSERT(machine.buttons.count <= MAX_NUMBER_OF_BUTTONS);


        // sort the buttons by witch ones give the most jolts.
        All_Buttons buttons_array = ZEROED;
        buttons_array.buttons_count = machine.buttons.count;
        for (u32 i = 0; i < machine.buttons.count; i++) {
            u16 button = machine.buttons.items[i];

            for (u32 j = 0; j < numbers_count; j++) {
                if (button & (1 << j)) {
                    buttons_array.b[i].bits[j] = 1;
                    buttons_array.b[i].bit_count += 1;
                }
            }
        }

        qsort(buttons_array.b, machine.buttons.count, sizeof(buttons_array.b[0]), compare_Button_Bit_Array);

        Counts final_counts = ZEROED;
        for (u32 i = 0; i < machine.voltages.count; i++) {
            final_counts.c[i] = machine.voltages.items[i];
        }


        Counts inital_counts = ZEROED;
        s32 min_presses = find_minimum_number_of_button_presses(&final_counts, &buttons_array, &inital_counts, 0, 0);
        ASSERT(min_presses != -1);
        // debug(min_presses);
        min_number_of_presses_for_volts += min_presses;
    }


    Solution solution = {
        .part_1 = total_number_of_presses,
        .part_2 = min_number_of_presses_for_volts,
    };
    return solution;
}


#define DAY "10"

int main(void) {
    printf("                             Solving Day %s!\n", DAY);
    printf("======================================================================================\n");

    Do_Example(7, 33);

    // Do_Input();

    // Perf_Input(Get_Input(), 10);

    printf("======================================================================================\n");
    Scratch_Free();
    return 0;
}

