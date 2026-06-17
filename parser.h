#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64

void parse_args(char *line, char **args);
void parse_full(char *line, char **args,
                char **infile, char **outfile, int *background);

#endif
