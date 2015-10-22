#include "mysh.h"

/*
   CITS2002 Project 2 2015
   Name(s):             student-name1 (, student-name2)
   Student number(s):   student-number-1 (, student-number-2)
   Date:                date-of-submission
 */

 int run_mysh(void)
 {
   interactive = (isatty(fileno(stdin)) && isatty(fileno(stdout)));

   int exitstatus;

   while (!feof(stdin))
   {
     CMDTREE	*t = parse_cmdtree(stdin);
     if (t != NULL)
     {
       exitstatus = execute_cmdtree(t);
       free_cmdtree(t);
     }
   }

   if(interactive)
   {
     fputc('\n', stdout);
   }

   return exitstatus;
 }

int main(int argc, char *argv[])
{
//  REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    argv0	= (argv0 = strrchr(argv[0],'/')) ? argv0+1 : argv[0];
    argc--;				// skip 1st command-line argument
    argv++;

//  INITIALIZE THE THREE INTERNAL VARIABLES
    char	*p;

    p		= getenv("HOME");
    HOME	= strdup(p == NULL ? DEFAULT_HOME : p);
    check_allocation(HOME);

    p		= getenv("PATH");
    PATH	= strdup(p == NULL ? DEFAULT_PATH : p);
    check_allocation(PATH);

    p		= getenv("CDPATH");
    CDPATH	= strdup(p == NULL ? DEFAULT_CDPATH : p);
    check_allocation(CDPATH);

//  DETERMINE IF THIS SHELL IS INTERACTIVE

    return run_mysh();
}
