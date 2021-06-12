#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define HOST_NAME_MAX 64
#define WHITESPACES " \t"
#define SPECIALS "|<>;&"
#define PATH_MAX 4096

#define READ_END 0
#define WRITE_END 1

#define STD_IN 0
#define STD_OUT 1

typedef int PIPE[2];

struct command_t
{
    char *name;
    int argc;
    char *argv[MAX_ARGS];
};

void intro();
int parsePath(char *[]);
void readCommand(char **);
char *lookupPath(char **, char **);
void printPrompt();
int parseCommand(char *, struct command_t *);
bool checkWhite(char);
bool checkSpecial(char);
int parsePath(char *[]);
void cd(struct command_t *);

#endif