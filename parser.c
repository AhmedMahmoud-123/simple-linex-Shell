#include <stdio.h>
#include <string.h>
#include "parser.h"

void parse_args(char *line, char **args) {
    int i = 0;
    char *token = strtok(line, " \t\r\n");
    while (token && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    args[i] = NULL;
}

void parse_full(char *line, char **args,
                char **infile, char **outfile, int *background) {
    *infile = *outfile = NULL;
    *background = 0;
    int argc = 0;

    char *token = strtok(line, " \t\r\n");
    while (token && argc < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) *infile = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) *outfile = token;
        } else if (strcmp(token, "&") == 0) {
            *background = 1;
        } else {
            size_t len = strlen(token);
            if (token[len - 1] == '&') {
                *background = 1;
                token[len - 1] = '\0';
                if (len > 1) args[argc++] = token;
            } else {
                args[argc++] = token;
            }
        }
        token = strtok(NULL, " \t\r\n");
    }
    args[argc] = NULL;
}
