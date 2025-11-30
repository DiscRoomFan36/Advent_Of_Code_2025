
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"

#define SRC_FOLDER          "./src/"
#define BUILD_FOLDER        "./build/"
#define THIRDPARTY_FOLDER   "./thirdparty/"

#define RAYLIB_FOLDER       THIRDPARTY_FOLDER"raylib-5.5/"



Cmd cmd = {0};


int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);


    bool debug        = false;
    bool run_all      = false;
    bool run_last_day = false;

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

    const char *days[] = {
        "day_01",
    };
    assert(ARRAY_LEN(days) > 0);


    for (int i = 0; i < ARRAY_LEN(days); i++) {
        const char *day = days[i];


        cmd_append(&cmd, "clang");
        cmd_append(&cmd, "-std=gnu11");

        cmd_append(&cmd, "-Wall", "-Wextra");
        cmd_append(&cmd, "-Werror");
        cmd_append(&cmd, "-I"THIRDPARTY_FOLDER);

        cmd_append(&cmd, "-ggdb");
        // cmd_append(&cmd, "-O2");
        cmd_append(&cmd, "-o",  temp_sprintf("%s%s", BUILD_FOLDER, "day_01"));
        cmd_append(&cmd, temp_sprintf("%s%s", SRC_FOLDER, "day_01.c"));
        if (!cmd_run(&cmd)) return 1;
    }


    #define DEBUGGER_PATH "/home/fletcher/Thirdpary/gf/gf2"


    if (run_last_day) {
        const char *last_day = days[ARRAY_LEN(days)-1];

        if (debug) cmd_append(&cmd, DEBUGGER_PATH);
        cmd_append(&cmd, temp_sprintf("%s%s", BUILD_FOLDER, last_day));

        if (!cmd_run(&cmd)) return 1;
    }

    if (run_all) {
        for (int i = 0; i < ARRAY_LEN(days); i++) {
            const char *day = days[i];

            if (debug) cmd_append(&cmd, DEBUGGER_PATH);
            cmd_append(&cmd, temp_sprintf("%s%s", BUILD_FOLDER, day));

            if (!cmd_run(&cmd)) return 1;
        }
    }

    return 0;
}
