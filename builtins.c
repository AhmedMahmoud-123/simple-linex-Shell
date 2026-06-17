#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

static char *history[MAX_HISTORY];
static int   hist_count = 0;

void history_add(const char *cmd) {
    if (hist_count < MAX_HISTORY) {
        history[hist_count++] = strdup(cmd);
    } else {
        free(history[0]);
        memmove(history, history + 1, (MAX_HISTORY - 1) * sizeof(char *));
        history[MAX_HISTORY - 1] = strdup(cmd);
    }
}

// cd
static int builtin_cd(char **args) {
    if (args[1] == NULL) {
        const char *home = getenv("HOME");
        if (home && chdir(home) != 0) perror("cd");
    } else {
        if (chdir(args[1]) != 0) perror("cd");
    }
    return 1;
}

// pwd
static int builtin_pwd(char **args) {
    (void)args;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) printf("%s\n", cwd);
    else perror("pwd");
    return 1;
}

// exit
static int builtin_exit(char **args) {
    int code = args[1] ? atoi(args[1]) : 0;
    printf("Goodbye!\n");
    exit(code);
}

// history 
static int builtin_history(char **args) {
    (void)args;
    for (int i = 0; i < hist_count; i++)
        printf("%4d  %s\n", i + 1, history[i]);
    return 1;
}

// Dispatch table
typedef struct { const char *name; int (*func)(char **); } Builtin;

static Builtin builtins[] = {
    { "cd",      builtin_cd      },
    { "pwd",     builtin_pwd     },
    { "exit",    builtin_exit    },
    { "history", builtin_history },
    { NULL, NULL }
};

int run_builtin(char **args) {
    for (int i = 0; builtins[i].name; i++)
        if (strcmp(args[0], builtins[i].name) == 0) {
            builtins[i].func(args);
            return 1;
        }
    return 0;
}
