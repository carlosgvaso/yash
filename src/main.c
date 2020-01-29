/*
 * Main functionality of the YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include <main.h>

/*
 * Point of entry.
 */
int main(int argc, char **argv) {
	int cpid;
	char *inString;
	char **parsedcmd;

	while ((inString = readline("# "))) {
		parsedcmd = parseString(inString);
		cpid = fork();

		if (cpid == 0) {
			execvp(parsedcmd[0], parsedcmd);
		} else {
			wait((int *) NULL);
		}
	}

	return (0);
}
