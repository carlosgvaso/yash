/**
 * @file main.c
 *
 * @brief Main functionality of the YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include <main.h>


/**
 * @brief Split a command string into string tokens.
 *
 * This function uses strtok() to split a command string into string tokens.
 * Tokens are considered to be contiguous characters separated by whitespace.
 *
 * @param	cmd	Command struct
 *
 * @sa strtok(), Cmd
 */
void tokenizeString(struct Cmd* cmd) {
	const char CMD_TOKEN_DELIM = ' ';	// From requirements
	int len = 0;

	// Remove final newline char and replace with NULL char
	len = strlen(cmd->cmd_str);
	if (cmd->cmd_str[len-1] == '\n') {
		cmd->cmd_str[len-1] = '\0';
	}

	// Break down the command into tokens using strtok
	cmd->cmd_tok[0] = strtok(cmd->cmd_str, &CMD_TOKEN_DELIM);
	int count = 1;	// Start at one because we already run strtok once

	while ((cmd->cmd_tok[count] = strtok(NULL, &CMD_TOKEN_DELIM))) {
		count++;
	}
	cmd->cmd_tok_len = count;
}

/**
 * @brief Parse a command.
 *
 * This function takes a raw command string, and parses it to load it into a
 * `Cmd` struct as per the requirements.
 *
 * TODO: Might need check for multiple redirections of the same type to raise an error.
 *
 * @param	cmd_str	Raw command string
 * @param	cmd		Command struct
 *
 * @sa	Cmd
 */
void parseCmd(char* cmd_str, struct Cmd* cmd) {
	const char I_REDIR_OPT[2] = "<\0";
	const char O_REDIR_OPT[2] = ">\0";
	const char E_REDIR_OPT[3] = "2>\0";
	const char BG_OPT[2] = "&\0";
	const char PIPE_OPT[2] = "|\0";
	const char SYNTAX_ERR_1[MAX_ERROR_LEN] = "syntax error: command should not"
			" start with \0";
	const char SYNTAX_ERR_2[MAX_ERROR_LEN] = "syntax error: near token \0";
	const char SYNTAX_ERR_3[MAX_ERROR_LEN] = "syntax error: command should not"
			" end with \0";
	const char SYNTAX_ERR_4[MAX_ERROR_LEN] = "syntax error: & should be the last"
			" token of the command\0";

	// Save and tokenize command string
	strcpy(cmd->cmd_str, cmd_str);
	tokenizeString(cmd);

	/*
	 * Iterate over all tokens to look for arguments, redirection directives,
	 * pipes and background directives
	 */
	int cmd_count = 0;	// Command array counter
	for (int i=0; i<cmd->cmd_tok_len; i++) {
		if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i])) {	// Check for input redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->err_msg, SYNTAX_ERR_1);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (i >= cmd->cmd_tok_len-1) {	// Check it is not the last token
				strcpy(cmd->err_msg, SYNTAX_ERR_3);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->err_msg, SYNTAX_ERR_2);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!cmd->pipe) {
					strcpy(cmd->in1, cmd->cmd_tok[i]);
				} else {
					strcpy(cmd->in2, cmd->cmd_tok[i]);
				}
			}
		} else if (!strcmp(O_REDIR_OPT,cmd->cmd_tok[i])) {	// Output redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->err_msg, SYNTAX_ERR_1);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (i >= cmd->cmd_tok_len-1) {	// Check it is not the last token
				strcpy(cmd->err_msg, SYNTAX_ERR_3);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->err_msg, SYNTAX_ERR_2);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!cmd->pipe) {
					strcpy(cmd->out1, cmd->cmd_tok[i]);
				} else {
					strcpy(cmd->out2, cmd->cmd_tok[i]);
				}
			}
		} else if (!strcmp(E_REDIR_OPT, cmd->cmd_tok[i])) {	// Error redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->err_msg, SYNTAX_ERR_1);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (i >= cmd->cmd_tok_len-1) {	// Check it is not the last token
				strcpy(cmd->err_msg, SYNTAX_ERR_3);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->err_msg, SYNTAX_ERR_2);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!cmd->pipe) {
					strcpy(cmd->err1, cmd->cmd_tok[i]);
				} else {
					strcpy(cmd->err2, cmd->cmd_tok[i]);
				}
			}
		} else if (!strcmp(PIPE_OPT, cmd->cmd_tok[i])) {	// Pipe command
			// Check if pipe token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->err_msg, SYNTAX_ERR_1);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (i >= cmd->cmd_tok_len-1) {	// Check it is not the last token
				strcpy(cmd->err_msg, SYNTAX_ERR_3);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->err_msg, SYNTAX_ERR_2);
				strcat(cmd->err_msg, cmd->cmd_tok[i]);
				return;
			} else {	// Correct syntax
				cmd->pipe = 1;
				cmd_count = 0;	// Start argument count for cmd2
			}
		} else if (!strcmp(BG_OPT, cmd->cmd_tok[i])) {	// Background command
			// Check if background token has the correct syntax
			if (i != cmd->cmd_tok_len-1) {	// Check if it is not the last token
				strcpy(cmd->err_msg, SYNTAX_ERR_4);
				return;
			} else {
				cmd->bg = 1;
			}
		} else {	// Command argument
			if (!cmd->pipe) {
				cmd->cmd1[cmd_count] = cmd->cmd_tok[i];
				cmd_count++;
			} else {
				cmd->cmd2[cmd_count] = cmd->cmd_tok[i];
				cmd_count++;
			}
		}
	}
}


/**
 * @brief Redirect input, output and error.
 *
 * @param	cmd	Command to set the redirection
 */
void redirectIO(struct Cmd* cmd) {
	const char REDIR_ERR_1[MAX_ERROR_LEN] = "open errno ";
	const char REDIR_ERR_2[MAX_ERROR_LEN] = ": could not open file: ";
	extern errno;
	char errno_str[sizeof(int)*8+1];

	// Input redirection
	if (strcmp(cmd->in1, EMPTY_STR)) {
		//
		int i1fd = open(cmd->in1, O_RDONLY,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (i1fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->in1);
			return;
		}

		dup2(i1fd, 0);
	}
/*
	if (strcmp(cmd->in2, EMPTY_STR)) {
		//
		int i2fd = open(cmd->in2, O_RDONLY,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (i2fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->in2);
			return;
		}

		dup2(i2fd, 0);
	}
*/
	// Output redirection
	if (strcmp(cmd->out1, EMPTY_STR)) {
		// Create output file
		int o1fd = open(cmd->out1, O_WRONLY|O_CREAT|O_TRUNC,
				S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (o1fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->out1);
			return;
		}

		dup2(o1fd, 1);
	}
/*
	if (strcmp(cmd->out2, EMPTY_STR)) {
		//
		int o2fd = open(cmd->out2, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (o2fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->out2);
			return;
		}

		dup2(o2fd, 1);
	}
*/
	// Error redirection
	if (strcmp(cmd->err1, EMPTY_STR)) {
		//
		int e1fd = open(cmd->err1, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (e1fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->err1);
			return;
		}

		dup2(e1fd, 2);
	}
/*
	if (strcmp(cmd->err2, EMPTY_STR)) {
		//
		int e2fd = open(cmd->err2, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (e2fd == -1) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->err2);
			return;
		}

		dup2(e2fd, 2);
	}
*/
}


/**
 * @brief Point of entry.
 *
 * @param argc	Number of command line arguments
 * @param argv	Array of command line arguments
 * @return	Errorcode
 */
int main(int argc, char **argv) {
	int cpid;
	char* cmd_str;

	while ((cmd_str = readline("# "))) {
		// Initialize a new Cmd struct
		struct Cmd cmd = {
				EMPTY_STR,		// cmd_str
				{ EMPTY_STR },	// cmd_tok
				0,			// cmd_tok_size
				{ EMPTY_STR },	// cmd1
				EMPTY_STR,		// in1
				EMPTY_STR,		// out1
				EMPTY_STR,		// err1
				{ EMPTY_STR },	// cmd2
				EMPTY_STR,		// in2
				EMPTY_STR,		// out2
				EMPTY_STR,		// err2
				0,			// pipe
				0,			// bg
				EMPTY_STR		// err_msg
		};

		parseCmd(cmd_str, &cmd);

		if (strcmp(cmd.err_msg, EMPTY_STR)) {
			printf("-yash: %s\n", cmd.err_msg);
			continue;
		}
		cpid = fork();

		if (cpid == 0) {
			// TODO: Add support for redirection
			redirectIO(&cmd);
			if (strcmp(cmd.err_msg, EMPTY_STR)) {
				printf("-yash: %s\n", cmd.err_msg);
				continue;
			}

			// TODO: Add support for pipes
			/*
			error = pipe(&cmd);
			if (strcmp(cmd.err_msg, EMPTY_STR)) {
				printf("-yash: %s\n", cmd.err_msg);
				continue;
			}*/
			// TODO: Add support for background cmds
			// TODO: Add support for job control
			execvp(cmd.cmd1[0], cmd.cmd1);
		} else {
			wait((int *) NULL);
		}
	}

	return (0);
}
