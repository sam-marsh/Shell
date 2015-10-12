#include "mysh.h"

/*
   CITS2002 Project 2 2015
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_cmdtree0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_cmdtree(CMDTREE *t)
{
    int  exitstatus;

    if(t == NULL) {			// hmmmm, a that's problem
	exitstatus	= EXIT_FAILURE;
    }
    else {				// normal, exit commands
	exitstatus	= EXIT_SUCCESS;
    }

    return exitstatus;
}
