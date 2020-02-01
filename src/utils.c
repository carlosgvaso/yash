/**
 * @file utils.c
 *
 * @brief Utility functions.
 *
 * @author: Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include <main.h>


/**
 * @brief Parse the tokenized command.
 *
 * This function assumes the first token is the command to run, and the rest of
 * the tokens are a mix of the command arguments and redirection instructions.
 *
 * @param	cmd_struct		Command struct
 *
 * @sa	Cmd
 */
void parseCmd(struct Cmd* cmd) {
	//const char WHITESPACE[2] = " \0";
	const char I_REDIR_OPT[2] = "<\0";
	const char O_REDIR_OPT[2] = ">\0";
	const char E_REDIR_OPT[3] = "2>\0";
	const char BG_OPT[2] = "&\0";
	const char PIPE_OPT[2] = "|\0";
	const char PARSING_ERR[MAX_ERROR_LEN] = "syntax error near unexpected token"
			"`newline'";	// This is the same error message that bash gives

	/*
	 * Assume the first entry is the command to run. The requirements state
	 * there will not be any redirection operators at the beginning of the
	 * command input string.
	 */
	cmd->cmd1[0] = cmd->cmd_tok[0];
	int arg_count = 1;

	/*
	 * Iterate over all tokens to look for arguments, redirection directives,
	 * pipes and background directives
	 */
	for (int i=1; i<cmd->cmd_tok_size-1; i++) {
		if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i])) {	// Check for input redir
			// Check if there is a token after the redirection
			if (i >= cmd->cmd_tok_size-1) {
				strcpy(cmd->parsing_err, PARSING_ERR);
			} else {
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->in, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(O_REDIR_OPT,cmd->cmd_tok[i])) {	// Output redir
			// Check if there is a token after the redirection
			if (i >= cmd->cmd_tok_size-1) {
				strcpy(cmd->parsing_err, PARSING_ERR);
			} else {
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->out, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(E_REDIR_OPT, cmd->cmd_tok[i])) {	// Error redir
			// Check if there is a token after the redirection
			if (i >= cmd->cmd_tok_size-1) {
				strcpy(cmd->parsing_err, PARSING_ERR);
			} else {
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->err, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(PIPE_OPT, cmd->cmd_tok[i])) {	// Pipe command
			// Check if there is a token after the pipe operator
			if (i >= cmd->cmd_tok_size-1) {
				strcpy(cmd->parsing_err, PARSING_ERR);
			} else {
				cmd->pipe = 1;
				i++;	// Move ahead one iteration to get the second command
				cmd->cmd2[0] = cmd->cmd_tok[i];
				arg_count = 1;	// Start argument count for cmd2
			}
		} else if (!strcmp(BG_OPT, cmd->cmd_tok[i])) {	// Background command
			// Check if there is a token after the background operator
			if (i < cmd->cmd_tok_size-1) {
				strcpy(cmd->parsing_err, PARSING_ERR);
			} else {
				cmd->bg = 1;
			}
		} else {	// Command argument
			if (!cmd->pipe) {
				cmd->cmd1[arg_count] = cmd->cmd_tok[i];
				arg_count++;
			} else {
				cmd->cmd2[arg_count] = cmd->cmd_tok[i];
			}
		}
	}
}

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
	cmd->cmd_tok_size = count;
}
