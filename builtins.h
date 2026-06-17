#ifndef BUILTINS_H
#define BUILTINS_H

#define MAX_HISTORY 100

void history_add(const char *cmd);
int  run_builtin(char **args);

#endif
