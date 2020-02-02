/**
 * @file utils.c
 *
 * @brief Utility functions.
 *
 * @author: Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
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
	cmd->cmd_tok_size = count;
}

/**
 * @brief Parse a command.
 *
 * This function takes a raw command string, and parses it to load it into a
 * `Cmd` struct as per the requirements.
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
	for (int i=0; i<cmd->cmd_tok_size; i++) {
		if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i])) {	// Check for input redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->parsing_err, SYNTAX_ERR_1);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (i >= cmd->cmd_tok_size-1) {	// Check it is not the last token
				strcpy(cmd->parsing_err, SYNTAX_ERR_3);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->parsing_err, SYNTAX_ERR_2);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->in, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(O_REDIR_OPT,cmd->cmd_tok[i])) {	// Output redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->parsing_err, SYNTAX_ERR_1);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (i >= cmd->cmd_tok_size-1) {	// Check it is not the last token
				strcpy(cmd->parsing_err, SYNTAX_ERR_3);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->parsing_err, SYNTAX_ERR_2);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->out, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(E_REDIR_OPT, cmd->cmd_tok[i])) {	// Error redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->parsing_err, SYNTAX_ERR_1);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (i >= cmd->cmd_tok_size-1) {	// Check it is not the last token
				strcpy(cmd->parsing_err, SYNTAX_ERR_3);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->parsing_err, SYNTAX_ERR_2);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				strcpy(cmd->err, cmd->cmd_tok[i]);
			}
		} else if (!strcmp(PIPE_OPT, cmd->cmd_tok[i])) {	// Pipe command
			// Check if pipe token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(cmd->parsing_err, SYNTAX_ERR_1);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (i >= cmd->cmd_tok_size-1) {	// Check it is not the last token
				strcpy(cmd->parsing_err, SYNTAX_ERR_3);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else if (!strcmp(I_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, cmd->cmd_tok[i+1]) ||
					!strcmp(BG_OPT, cmd->cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(cmd->parsing_err, SYNTAX_ERR_2);
				strcat(cmd->parsing_err, cmd->cmd_tok[i]);
				break;
			} else {	// Correct syntax
				cmd->pipe = 1;
				cmd_count = 0;	// Start argument count for cmd2
			}
		} else if (!strcmp(BG_OPT, cmd->cmd_tok[i])) {	// Background command
			// Check if background token has the correct syntax
			if (i != cmd->cmd_tok_size-1) {	// Check if it is not the last token
				strcpy(cmd->parsing_err, SYNTAX_ERR_4);
				break;
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
