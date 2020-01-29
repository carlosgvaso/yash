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
	char *in_string;
	char **parsed_cmd;

	while ((in_string = readline("# "))) {
		parsed_cmd = parseString(in_string);
		cpid = fork();

		if (cpid == 0) {
			execvp(parsed_cmd[0], parsed_cmd);
		} else {
			wait((int *) NULL);
		}
		free(parsed_cmd);
	}

	return (0);
}
