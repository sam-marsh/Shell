#include "mysh.h"

/*
   CITS2002 Project 2 2015
   Name(s):             Samuel Marsh, Liam Reeves	
   Student number(s):   21324325, 21329882
   Date:                TODO		        
 */

extern char **environ;  //TODO - do we actually need this?



// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_cmdtree0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_cmdtree(CMDTREE *t)
{
    int  exitstatus;
    if (t == NULL) {			// hmmmm, a that's problem
	    exitstatus	= EXIT_FAILURE;
    }
    else {				// normal, exit command
      switch (t->type)
      {
        case N_AND:
          {
            exitstatus = execute_cmdtree(t->left);
            if (exitstatus == EXIT_SUCCESS)
              exitstatus = execute_cmdtree(t->right);
            break;
          }
        case N_BACKGROUND:
          {
            int pid;
            switch (pid = fork())
            {
              case -1:
                perror("fork()");
                exitstatus = EXIT_FAILURE;
                break;
              case 0:
                execute_cmdtree(t->left);
                break;
              default:
                execute_cmdtree(t->right);
                exitstatus = EXIT_SUCCESS;
                break;
            }
            break;
          }
        case N_OR:
          {
            exitstatus = execute_cmdtree(t->left);
            if (exitstatus == EXIT_FAILURE)
                exitstatus = execute_cmdtree(t->right);
            break;
          }
        case N_SEMICOLON:
          {
            execute_cmdtree(t->left);
            exitstatus = execute_cmdtree(t->right);
            break;
          }
        case N_PIPE:
          {
            int fd[2];
            int pid;

            pipe(fd);

            switch (pid = fork())
            {
              case -1:
                perror("fork()");
                exitstatus = EXIT_FAILURE;
                break;
              case 0:
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                exit(execute_cmdtree(t->left));
                break;
              default:
                {
                  int pid_2;

                  switch (pid_2 = fork())
                  {
                    case -1:
                      perror("fork()");
                      exitstatus = EXIT_FAILURE;
                      break;
                    case 0:
                      dup2(fd[0], STDIN_FILENO);
                      close(fd[0]);
                      close(fd[1]);
                      exit(execute_cmdtree(t->right));
                      break;
                    default:
                      wait(&exitstatus);
                      break;
                  }
                }
                break;
            }
            break;
          }
        case N_SUBSHELL:
          {
            int pid;
            switch (pid = fork())
            {
              case -1:
                perror("fork()");
                exitstatus = EXIT_FAILURE;
                break;
              case 0:
                execute_cmdtree(t->left);
                break;
              default:
                wait(&exitstatus);
                break;
            }
            break;
          }
        case N_COMMAND:
          {
            if (strcmp(t->argv[0], "set") == 0)
            {
              char *var_name = t->argv[1];
              char *var_val = strdup(t->argv[2]);
              if (strcmp(var_name, "PATH") == 0)
              {
                PATH = var_val;
              }
              else if (strcmp(var_name, "HOME") == 0)
              {
                HOME = var_val;
              }
              else if (strcmp(var_name, "CDPATH") == 0)
              {
                CDPATH = var_val;
              }
              else
              {
                //TODO error
              }
            }
            else if (strcmp(t->argv[0], "exit") == 0)
            {
              if (t->argv[1] != NULL)
              {
                exit(atoi(t->argv[1]));
              }
              else
              {
                exit(EXIT_SUCCESS); //TODO
              }
            }
            else if (strcmp(t->argv[0], "cd") == 0)
            {
              if (t->argv[1] != NULL)
              {
                if (strchr(t->argv[1], '/'))
                {
                  chdir(t->argv[1]);
                }
                else
                {
                  char *token = strtok(CDPATH, ":");
                  while (token != NULL)
                  {
                    char full_path[MAXPATHLEN];
                    sprintf(full_path, "%s/%s", token, t->argv[1]);
                    struct stat s;
                    int err = stat(full_path, &s);
                    if (err != -1 && S_ISDIR(s.st_mode))
                    {
                      chdir(full_path);
                      break;
                    }
                    token = strtok(NULL, ":");
                  }
                }
              }
              else
              {
                chdir(HOME);
              }
            }
            else
            {
              bool time = strcmp(t->argv[0], "time") == 0;
              char **argv = t->argv;
              int argc = t->argc;
              if (time)
              {
                ++argv;
                --argc;
              }

              int pid;
              switch (pid = fork())
              {
                case -1:
                  perror("fork()");
                  exitstatus = EXIT_FAILURE;
                  break;
                case 0:
                  {
                    char *file_path;
                    if (strchr(argv[0], '/'))
                    {
                      file_path = strdup(argv[0]);
                    }
                    else 
                    {
                      char *token = strtok(PATH, ":");
                      while (token != NULL)
                      {
                        char full_path[MAXPATHLEN];
                        sprintf(full_path, "%s/%s", token, argv[0]);
                        if (access(full_path, F_OK) != -1)
                        {
                          file_path = strdup(full_path);
                          break;
                        }
                        token = strtok(NULL, ":");
                      }
                    }
                    if (t->infile != NULL)
                    {
                      FILE *fp = fopen(t->infile, "r");
                      if (fp == NULL) 
                      { 
                        //error (TODO) 
                      }
                      dup2(fileno(fp), STDIN_FILENO);
                      fclose(fp);
                    }
                    if (t->outfile != NULL)
                    {
                      FILE *fp = fopen(t->outfile, t->append ? "a" : "w");
                      if (fp == NULL)
                      {
                        //error (TODO)
                      }
                      dup2(fileno(fp), STDOUT_FILENO);
                      fclose(fp);
                    }
                    if (execve(file_path, argv, environ) == -1)
                    {
                      FILE *fp = fopen(file_path, "r");
                      if (fp == NULL)
                      {
                        //error (TODO)
                      }

                      int pid;

                      switch (pid = fork())
                      {
                        case -1:
                          perror("fork()");
                          exitstatus = EXIT_FAILURE;
                          break;
                        case 0:
                          dup2(fileno(fp), STDIN_FILENO);
                          break;
                        default:
                          wait(&exitstatus);
                          break;
                      }

                      fclose(fp);
                    }
                    break;
                  }
                default:
                  if (time)
                  {
                    struct timeval st_start;
                    struct timeval st_end;
                    int start = gettimeofday(&st_start, NULL);
                    if (start == -1)
                    {
                      //error TODO
                    }
                    wait(&exitstatus);
                    int end = gettimeofday(&st_end, NULL);
                    if (end == -1)
                    {
                      //error TODO
                    }
                    fprintf(stderr, "%limsec\n", (st_end.tv_usec - st_start.tv_usec) / 1000);
                  }
                  else
                  {
                    wait(&exitstatus);
                  }
                  break;
              }
            }
            break;
          }
        default:
          fprintf(stderr, "unknown node type\n");
          return EXIT_FAILURE;
      }
    }
    return exitstatus;
}
