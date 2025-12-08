
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"

#define SRC_FOLDER          "./src/"
#define BUILD_FOLDER        "./build/"
#define THIRDPARTY_FOLDER   "./thirdparty/"

#define RAYLIB_FOLDER       THIRDPARTY_FOLDER"raylib-5.5/"


int day_sorting_function(const void *_a, const void *_b) {
    String_View a = *(String_View*)_a;
    String_View b = *(String_View*)_b;

    a.data += a.count - 2; // only the last 2 chars
    a.count = 2;

    b.data += b.count - 2; // only the last 2 chars
    b.count = 2;

    int a_num = atoi(nob_temp_sv_to_cstr(a));
    int b_num = atoi(nob_temp_sv_to_cstr(b));

    return a_num-b_num;
}


Cmd cmd = {0};

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);


    bool debug             = false;
    bool run_all           = false;
    bool run_last_day      = false;
    bool turn_on_optimizer = false;

    // handle arguments
    for (int i = 1; i < argc; i++) {
        const char *text = argv[i];
        if        (strcmp(text, "debug") == 0) {
            debug = true;
        } else if (strcmp(text, "run") == 0) {
            if (i+1 < argc) {
                i += 1;
                const char *text_2 = argv[i];
                assert(strcmp(text_2, "last_day") == 0);
                run_last_day = true;
            } else {
                run_all = true;
            }
        } else if (strcmp(text, "optimize") == 0) {
            turn_on_optimizer = true;
        }
    }


    // TODO make this a command line arg
    #define BESTED_PATH "/home/fletcher/Programming/C-things/Bested.h/Bested.h"
    if (file_exists(BESTED_PATH) == 1) {
        cmd_append(&cmd, "cp");
        cmd_append(&cmd, BESTED_PATH);
        cmd_append(&cmd, THIRDPARTY_FOLDER"Bested.h");
        if (!cmd_run(&cmd)) return 1;
    }

    mkdir_if_not_exists(BUILD_FOLDER);


    Nob_File_Paths src_folder_files = {};
    if (!read_entire_dir(SRC_FOLDER, &src_folder_files)) return 1;


     typedef struct {
        String_View *items;
        size_t count;
        size_t capacity;
    } String_Array;


    String_Array days = {};
    for (int i = 0; i < src_folder_files.count; i++) {
        String_View file = sv_from_cstr(src_folder_files.items[i]);
        if (file.data[0] == '.')      continue;
        if (!sv_end_with(file, ".c")) continue;
        file.count -= 2; // remove the extention.
        da_append(&days, file);
    }
    assert(days.count > 0);

    // sort days by number.
    qsort(days.items, days.count, sizeof(days.items[0]), day_sorting_function);

    Procs procs = {0};
    for (int i = 0; i < days.count; i++) {
        String_View day = days.items[i];


        cmd_append(&cmd, "clang");
        // cmd_append(&cmd, "-std=gnu11");

        cmd_append(&cmd, "-Wall", "-Wextra");
        // cmd_append(&cmd, "-Werror");
        cmd_append(&cmd, "-I"THIRDPARTY_FOLDER);

        // why not?
        cmd_append(&cmd, "-march=native");

        if (turn_on_optimizer) cmd_append(&cmd, "-O2");
        else                   cmd_append(&cmd, "-ggdb");


        cmd_append(&cmd, "-o",  temp_sprintf("%s"SV_Fmt, BUILD_FOLDER, SV_Arg(day)));
        cmd_append(&cmd, temp_sprintf("%s"SV_Fmt".c", SRC_FOLDER, SV_Arg(day)));
        if (!cmd_run(&cmd, .async = &procs)) return 1;
    }

    if (!procs_wait(procs)) return 1;


    #define DEBUGGER_PATH "/home/fletcher/Thirdpary/gf/gf2"



#define ONLY_RUN_THIS_DAY "day_08"

#ifdef ONLY_RUN_THIS_DAY

    if (run_last_day | run_all) {
        String_View day = sv_from_cstr(ONLY_RUN_THIS_DAY);

        if (debug) cmd_append(&cmd, DEBUGGER_PATH);
        cmd_append(&cmd, temp_sprintf("%s"SV_Fmt, BUILD_FOLDER, SV_Arg(day)));

        if (!cmd_run(&cmd)) return 1;
    }

#else


    if (run_last_day) {
        String_View last_day = days.items[days.count-1];

        if (debug) cmd_append(&cmd, DEBUGGER_PATH);
        cmd_append(&cmd, temp_sprintf("%s"SV_Fmt, BUILD_FOLDER, SV_Arg(last_day)));

        if (!cmd_run(&cmd)) return 1;
    }

    if (run_all) {
        for (int i = 0; i < days.count; i++) {
            String_View day = days.items[i];

            if (debug) cmd_append(&cmd, DEBUGGER_PATH);
            cmd_append(&cmd, temp_sprintf("%s"SV_Fmt, BUILD_FOLDER, SV_Arg(day)));

            if (!cmd_run(&cmd)) return 1;
        }
    }

#endif

    return 0;
}
