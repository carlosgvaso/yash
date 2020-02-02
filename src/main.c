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
		// Initialize a new Cmd struct
		struct Cmd cmd = {
				"\0",		// cmd_str
				{ "\0" },	// cmd_tok
				0,			// cmd_tok_size
				{ "\0" },	// cmd1
				"\0",		// in1
				"\0",		// out1
				"\0",		// err1
				{ "\0" },	// cmd2
				"\0",		// in2
				"\0",		// out2
				"\0",		// err2
				0,			// pipe
				0,			// bg
				"\0"		// parsing_err
		};

		parseCmd(cmd_str, &cmd);

		if (strcmp(cmd.parsing_err, "\0")) {
			printf("-yash: %s\n", cmd.parsing_err);
			continue;
		}
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
