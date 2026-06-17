#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include "builtins.h"
#include "parser.h"

#define MAX_INPUT 1024

/* PID of the current foreground child, 0 if none */
pid_t foreground_pid = 0;

/* Ctrl+C handler - kills the foreground child only, never the shell */
void handle_sigint(int sig) {
    (void)sig;
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT); /* kill child */
        foreground_pid = 0;
        tcsetpgrp(STDIN_FILENO, getpgrp()); /* give terminal back to shell */
    }
    write(STDOUT_FILENO, "\n^C\n", 4); /* main loop will reprint prompt */
}

/* Runs a single command with optional redirection and background support */
void execute_command(char **args, int background, char *infile, char *outfile) {
    if (args[0] == NULL) return;

    /* try built-ins first, if matched no need to fork */
    if (run_builtin(args) == 1) return;

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) {
        /* child process */

        /* reset SIGINT so child can be killed by Ctrl+C */
        signal(SIGINT, SIG_DFL);

        /* input redirection: read from file instead of keyboard */
        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) { perror(infile); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        /* output redirection: write to file instead of terminal */
        if (outfile) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror(outfile); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        /* replace child process with the requested program */
        execvp(args[0], args);

        /* only reached if execvp failed */
        fprintf(stderr, "myShell: %s: command not found\n", args[0]);
        exit(1);
    }

    /* parent process */
    if (background) {
        /* background: don't wait, just print PID */
        printf("[background] PID %d\n", pid);
    } else {
        /* foreground: save PID then wait for child to finish */
        foreground_pid = pid;
        int status;
        waitpid(pid, &status, 0);
        foreground_pid = 0;
        tcsetpgrp(STDIN_FILENO, getpgrp()); /* give terminal back to shell */
    }
}

/* Runs two commands connected by a pipe, example: ls | grep myShell */
void execute_pipe(char **args1, char **args2) {
    int pipefd[2]; /* pipefd[0] = read end, pipefd[1] = write end */
    if (pipe(pipefd) < 0) { perror("pipe"); return; }

    /* left side: writes its output into the pipe */
    pid_t p1 = fork();
    if (p1 < 0) { perror("fork"); return; }
    if (p1 == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); /* stdout goes into pipe */
        close(pipefd[1]);
        execvp(args1[0], args1);
        fprintf(stderr, "myShell: %s: command not found\n", args1[0]);
        exit(1);
    }

    /* right side: reads its input from the pipe */
    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); return; }
    if (p2 == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); /* stdin comes from pipe */
        close(pipefd[0]);
        execvp(args2[0], args2);
        fprintf(stderr, "myShell: %s: command not found\n", args2[0]);
        exit(1);
    }

    /* parent closes pipe ends and waits for both children */
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

int main(void) {
    char input[MAX_INPUT];

    /* register Ctrl+C handler, SA_RESTART is off so fgets gets
       interrupted by the signal and we can reprint the prompt */
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* make shell its own process group so Ctrl+C only hits the child */
    setpgrp();
    tcsetpgrp(STDIN_FILENO, getpgrp());

    while (1) {
        printf("myShell> ");
        fflush(stdout);

        /* read one line, if interrupted by signal retry the loop */
        if (!fgets(input, MAX_INPUT, stdin)) {
            if (errno == EINTR) { clearerr(stdin); continue; }
            printf("\n");
            break; /* Ctrl+D exits the shell */
        }

        /* remove trailing newline */
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        /* save command to history */
        history_add(input);

        /* check for pipe and handle it */
        char *pipe_pos = strchr(input, '|');
        if (pipe_pos) {
            *pipe_pos = '\0';
            char *args1[MAX_ARGS], *args2[MAX_ARGS];
            parse_args(input,        args1);
            parse_args(pipe_pos + 1, args2);
            if (args1[0] && args2[0])
                execute_pipe(args1, args2);
            continue;
        }

        /* parse for args, redirection and background then execute */
        char *args[MAX_ARGS];
        char *infile = NULL, *outfile = NULL;
        int background = 0;

        parse_full(input, args, &infile, &outfile, &background);
        if (args[0] == NULL) continue;

        execute_command(args, background, infile, outfile);
    }
    return 0;
}
