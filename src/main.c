#include "minishell.h"

PIPE pip1, pip2;
bool choose = true;
#define PIPE1 (choose ? pip1 : pip2)
#define PIPE2 (choose ? pip2 : pip1)
int main()
{
  /* Shell initialization */

  intro();

  char **pathv = (char **)calloc(MAX_PATHS, sizeof(char *));
  int totalPaths = parsePath(pathv); /* Get directory paths from PATH */

  while (true)
  {
    printPrompt();

    /* Read the command line and parse it */
    char *commandLine = NULL;
    readCommand(&commandLine);
    struct command_t command = {0, 0, NULL};
    command.argc = parseCommand(commandLine, &command);

    if (command.argc)
    {
      // Checking for cd command
      if (!strcmp(command.argv[0], "cd"))
        cd(&command);
      else if (!strcmp(command.argv[0], "exit"))
      {
        //builtin exit command implementation
        if (command.argc > 2)
          fprintf(stderr, "shell: cd: too many arguments\n");
        else
        {
          /* Shell termination */
          int x;
          if (command.argv[1])
            x = atoi(command.argv[1]);
          for (int r = 0; r < totalPaths; r++)
            if (pathv[r])
              free(pathv[r]), pathv[r] = NULL;

          if (pathv)
            free(pathv), pathv = NULL;

          exit(x);
        }
      }
      else if (strlen(command.argv[0]))
      {
        for (int i = 0; i < command.argc; i++)
        {
          /* Get the full pathname for the file */
          command.name = lookupPath(command.argv + i, pathv);
          if (command.name)
          {
            int type = -1;
            int k = 0;
            int l = 0;
            for (k = i; type == -1 && k < command.argc; k++)
              if (!strcmp((command.argv + i)[k - i], ";") || !strcmp((command.argv + i)[k - i], "&"))
                type = 1;
              else if (!strcmp((command.argv + i)[k - i], "|"))
                type = 2;
              else if (!strcmp((command.argv + i)[k - i], "<") || !strcmp((command.argv + i)[k - i], "<<") || !strcmp((command.argv + i)[k - i], ">") || !strcmp((command.argv + i)[k - i], ">>"))
              {
                type = 3;
                for (l = k; l < command.argc && (!strcmp((command.argv + i)[l - i], "<") || !strcmp((command.argv + i)[l - i], "<<") || !strcmp((command.argv + i)[l - i], ">") || !strcmp((command.argv + i)[l - i], ">>")); l += 2)
                  ;
              }

            if (type != -1)
              k--;

            char *parsedCommand[k - i + 1];
            for (int j = 0; j < k - i; j++)
            {
              parsedCommand[j] = (char *)malloc(strlen((command.argv + i)[j]) + 1);
              strcpy(parsedCommand[j], (command.argv + i)[j]);
            }
            parsedCommand[k - i] = NULL;

            if (type == 2 || type == 3)
              pipe(PIPE1);

            pid_t pid = fork();
            if (!pid)
            {
              int fdw = -5;
              int fdr = -5;

              if (type == 3)
              {
                for (int a = k; a < l; a += 2)
                {
                  char *filename = (char *)malloc(strlen(command.argv[a + 1]) + 1);
                  strcpy(filename, command.argv[a + 1]);
                  if (!strcmp(command.argv[a], "<") || !strcmp(command.argv[a], "<<"))
                  {
                    fdr = open(filename, O_RDONLY | O_NONBLOCK, 00700);
                    if (fdr == -1)
                      fprintf(stderr, "shell: %s: No such file or directory\n", filename);
                    else
                      dup2(fdr, STD_IN); //input redirect
                  }
                  else
                  {
                    if (!strcmp(command.argv[a], ">>"))
                      fdw = open(filename, O_RDWR | O_CREAT | O_APPEND | O_NONBLOCK, 00700);
                    else
                      fdw = creat(filename, 00700);

                    dup2(fdw, STD_OUT); //output redirect
                  }
                  free(filename);
                }

                if (fdw == -5 && l < command.argc && strcmp(command.argv[l], "|"))
                  fcntl(PIPE1[WRITE_END], F_SETFL, O_NONBLOCK), dup2(PIPE1[WRITE_END], STD_OUT); //output redirect
              }

              if (type == 2)
                fcntl(PIPE1[WRITE_END], F_SETFL, O_NONBLOCK), dup2(PIPE1[WRITE_END], STD_OUT); //output redirect

              if (i - 1 >= 0 && !strcmp(*(command.argv + i - 1), "|"))
                fcntl(PIPE2[READ_END], F_SETFL, O_NONBLOCK), dup2(PIPE2[READ_END], STD_IN); //input redirect

              if (fdr != -1)
                if (execv(command.name, parsedCommand) == -1)
                  if (command.name)
                    fprintf(stderr, "%s: command not found\n", parsedCommand[0]);

              exit(0);
            }

            if (type == 2 || type == 3)
              close(PIPE1[WRITE_END]);

            if (i - 1 >= 0 && !strcmp(*(command.argv + i - 1), "|"))
              close(PIPE2[READ_END]);

            if (strcmp(command.argv[command.argc - 1], "&"))
              wait(NULL);

            choose = !choose;

            for (int j = 0; j < k - i; j++)
              if (parsedCommand[j])
                free(parsedCommand[j]), parsedCommand[j] = NULL;

            if (type == 3)
              i = l;
            else
              i = k;
          }
          else
            break;
        }
      }
    }

    for (int r = 0; r < command.argc; r++)
      if (command.argv[r])
        free(command.argv[r]), command.argv[r] = NULL;

    command.argc = 0;

    if (command.name)
      free(command.name), command.name = NULL;

    if (commandLine)
      free(commandLine), commandLine = NULL;
  }
}
