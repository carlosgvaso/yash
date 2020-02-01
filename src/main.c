/**
 * @file main.c
 *
 * @brief Main functionality of the YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include <main.h>

/**
 * @brief Point of entry.
 */
int main(int argc, char **argv) {
	int cpid;
	char* cmd_str;

	while ((cmd_str = readline("# "))) {
		struct Cmd cmd = {
				"\0",		// cmd_str
				{ "\0" },	// cmd_tok
				0,			// cmd_tok_size
				{ "\0" },	// cmd1
				{ "\0" },	// cmd2
				0,			// pipe
				0,			// bg
				"\0",		// in
				"\0",		// out
				"\0",		// err
				"\0"		// parsing_err
		};
		strcpy(cmd.cmd_str, cmd_str);
		tokenizeString(&cmd);
		parseCmd(&cmd);
		cpid = fork();

		if (cpid == 0) {
			// TODO: Add support for redirection, pipes and background cmds
			execvp(cmd.cmd1[0], cmd.cmd1);
		} else {
			wait((int *) NULL);
		}
	}

	return (0);
}
