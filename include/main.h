/*
 * Main functionality of the YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso
 */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_CMD_LEN 2000	// From requirements
#define MAX_TOKEN_LEN 30	// From requirements
#define MAX_TOKEN_NUM 1000	// MAX_CMD_LEN / 2 = 1000. The 2 comes from the min token size (1), plus a whitespace char as token delimiter.
#define MAX_CONCURRENT_JOBS 20	// From requirements


char** parseString(char * str);
