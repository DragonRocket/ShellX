#include "minishell.h"

void intro()
{
  pid_t blank = fork();

  if (!blank)
  {
    execlp("/usr/bin/clear", "clear", "-x", NULL);
    exit(0);
  }
  wait(NULL);

  //intro
  printf("\nShellX 2.0.0\n\n");
}

void cd(struct command_t *command)
{
  //builtin cd command implementation
  if (command->argc > 2)
    fprintf(stderr, "shell: cd: too many arguments\n");
  else
  {
    if (!command->argv[1])
    {
      chdir(getenv("HOME"));
    }
    else if (*command->argv[1] == '$')
    {
      int s = strlen(command->argv[1]);

      char c[s];

      for (int i = 0; i < s; i++)
        c[i] = command->argv[1][i + 1];

      char *p = (char *)getenv(c);

      if (!p)
        p = getenv("HOME");

      chdir(p);
    }
    else if (chdir(command->argv[1]) == -1)
      fprintf(stderr, "shell: cd: %s: No such file or directory\n", command->argv[1]);
  }
}

int parsePath(char *dirs[])
{
  /* This function reads the PATH variable for this
 * environment, then builds an array, dirs[], of the
 * directories in PATH
 */
  char *pathEnvVar = NULL;
  char *thePath = NULL;

  for (int i = 0; i < MAX_PATHS; i++)
    dirs[i] = NULL;
  pathEnvVar = (char *)getenv("PATH");
  thePath = (char *)malloc(strlen(pathEnvVar) + 1);
  strcpy(thePath, pathEnvVar);

  /* Loop to parse thePath. Look for a ":"
 * delimiter between each path name.
 */
  int i, j = 0, z = 0, k, n = 0;
  for (i = 0; i < MAX_PATHS * MAX_PATH_LEN; i++)
  {
    if (thePath[i] == ':' || thePath[i] == '\0')
    {
      dirs[z] = (char *)malloc(i - j + 2);
      for (k = j, n = 0; k < i; k++)
        dirs[z][n++] = thePath[k];
      dirs[z][n] = '/';
      dirs[z][n + 1] = '\0';
      j = k + 1;
      z++;

      if (thePath[i] == '\0')
        break;
    }
  }

  free(thePath), thePath = NULL;

  return z;
}

bool checkWhite(char ch)
{
  for (int i = 0; i < strlen(WHITESPACES); i++)
    if (WHITESPACES[i] == ch)
      return true;
  return false;
}

bool checkSpecial(char ch)
{
  for (int i = 0; i < strlen(SPECIALS); i++)
    if (SPECIALS[i] == ch)
      return true;
  return false;
}

int parseCommand(char *commandLine, struct command_t *command)
{
  int c = 0;
  for (int i = 0; i < strlen(commandLine); i++)
  {
    if (checkSpecial(commandLine[i]))
    {
      if (i + 1 < strlen(commandLine) && commandLine[i] == '<' && commandLine[i + 1] == '<')
      {
        command->argv[c] = (char *)malloc(3);
        strcpy(command->argv[c], "<<");
      }
      else if (i + 1 < strlen(commandLine) && commandLine[i] == '>' && commandLine[i + 1] == '>')
      {
        command->argv[c] = (char *)malloc(3);
        strcpy(command->argv[c], ">>");
      }
      else if (i + 1 < strlen(commandLine) && commandLine[i] == '&' && commandLine[i + 1] == '&')
      {
        command->argv[c] = (char *)malloc(3);
        strcpy(command->argv[c], "&&");
      }
      else if (i + 1 < strlen(commandLine) && commandLine[i] == '|' && commandLine[i + 1] == '|')
      {
        command->argv[c] = (char *)malloc(3);
        strcpy(command->argv[c], "||");
      }
      else
      {
        command->argv[c] = (char *)malloc(2);
        command->argv[c][0] = commandLine[i];
        command->argv[c][1] = '\0';
        i--;
      }

      c++;
      i += 1;
    }
    else if (!checkWhite(commandLine[i]))
    {
      int dQuote = 0;
      int sQuote = 0;
      bool exit = false;
      int j;

      for (j = i; !exit && commandLine[j] != '\0' && !checkSpecial(commandLine[j]); j++)
      {
        if (commandLine[j] == '\"')
          dQuote++;
        else if (commandLine[j] == '\'')
          sQuote++;
        else if (checkWhite(commandLine[j]))
        {
          if (!(sQuote % 2) && !(dQuote % 2))
            exit = true, j--;
        }
      }

      command->argv[c] = (char *)malloc(j - i + 1 - dQuote - sQuote);

      int n = 0;
      for (int k = i; k < j; k++)
        if (commandLine[k] != '\'' && commandLine[k] != '\"')
          command->argv[c][n++] = commandLine[k];

      command->argv[c][n] = '\0';
      c++;
      i = j - 1;
    }
  }
  command->argv[c] = NULL;
  return c;
}

void printPrompt()
{
  /* Build the prompt string to have the machine name,
 * current directory, or other desired information.
 */

  char hostname[HOST_NAME_MAX + 1];
  gethostname(hostname, HOST_NAME_MAX + 1);

  char *userHome = (char *)getenv("HOME");
  char *user = (char *)getenv("USER");

  char *pwd = malloc(PATH_MAX);
  getcwd(pwd, PATH_MAX);

  int i = 0;
  bool sub = true;
  for (i = 0; i < strlen(userHome) && sub; i++)
    if (pwd[i] != userHome[i])
      sub = false;

  char *cwdp = NULL;
  if (sub && (pwd[i] == '/' || pwd[i] == '\0'))
  {
    cwdp = (char *)malloc(strlen(pwd) - strlen(userHome) + 3);
    cwdp[0] = '~';
    cwdp[1] = '/';
    int j;
    for (j = 2; j < strlen(pwd) - strlen(userHome) + 1; j++)
    {
      cwdp[j] = pwd[j + strlen(userHome) - 1];
    }
    cwdp[j] = '\0';

    if (pwd)
      free(pwd), pwd = NULL;
  }
  else
    cwdp = pwd;

  printf("\033[1;31m┌──\033[0m");
  printf("\033[1;36m(\033[0m");
  printf("\033[1;32m%s\033[0m", user);
  printf("\033[1;34m@\033[0m");
  printf("\033[1;32m%s\033[0m", hostname);
  printf("\033[1;36m)\033[0m");
  printf("\033[1;31m-\033[0m");
  printf("\033[1;36m[\033[0m");
  printf("\033[4;33m%s\033[0m", cwdp);
  printf("\033[1;36m]\033[0m");
  printf("\033[1;31m\n└─\033[0m");
  printf("\033[1;36m{\033[0m");
  printf("\033[1;35m$\033[0m");
  printf("\033[1;33m#\033[0m");
  printf("\033[1;35m$\033[0m");
  printf("\033[1;36m}\033[0m");
  printf("\033[1;37m:\033[0m");
  printf("\033[1;31m>\033[0m ");
  fflush(stdout);
  free(cwdp), cwdp = NULL;
}

char *lookupPath(char **argv, char **dir)
{
  /* This function searches the directories identified by the dir
 * argument to see if argv[0] (the file name) appears there.
 * Allocate a new string, place the full path name in it, then
 * return the string.
 */
  char *result = NULL;
  char pName[MAX_PATH_LEN] = "\0";

  // Check to see if file name is already an absolute path
  if (*argv[0] == '/')
  {
    result = (char *)malloc(strlen(argv[0]) + 1);
    strcpy(result, argv[0]);
    return result;
  }

  // Look in current working directory
  strcpy(pName, argv[0]);
  result = (char *)malloc(strlen(pName) + 3);
  strcpy(result, "./");
  strcat(result, pName);
  if (!access(result, F_OK))
    return result;
  free(result), result = NULL;

  // Look in PATH directories.
  // Use access() to see if the file is in a dir.
  for (int i = 0; i < MAX_PATHS; i++)
  {
    if (dir[i] != NULL)
    {
      strcpy(pName, argv[0]);
      result = (char *)malloc(strlen(pName) + strlen(dir[i]) + 1);
      strcpy(result, dir[i]);
      strcat(result, pName);
      if (!access(result, F_OK))
        return result;
      free(result), result = NULL;
    }
    else
      break;
  }

  // File name not found in any path variable
  if (checkSpecial(*argv[0]))
    fprintf(stderr, "shell: syntax error near unexpected token `%s'\n", argv[0]);
  else
    fprintf(stderr, "%s: command not found\n", argv[0]);

  return NULL;
}

void readCommand(char **buffer)
{
  /* This code uses any set of I/O functions, such as those in the
 * stdio library to read the entire command line into the buffer. This
 * implementation is greatly simplified but it does the job.
 */

  size_t size = 0;
  char *tmp = NULL;

  size = getline(&tmp, &size, stdin);
  tmp = (char *)realloc(tmp, size);
  tmp[size - 1] = '\0';

  *buffer = tmp;
}