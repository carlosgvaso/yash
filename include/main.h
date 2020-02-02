/**
 * @file  main.h
 *
 * @brief Main functionality of the YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_CMD_LEN 2000	//! Max command length as per requirements
#define MAX_TOKEN_LEN 30	//! Max token length as per requirements
#define MAX_ERROR_LEN 256	//! Max error message length
/**
 * @brief Max number of tokens per command.
 *
 * This is calculated as follows:
 *
 * \f[
 * \dfrac{MAX_CMD_LEN}{2} = 1000
 * \f]
 *
 * The 2 comes from the min token size (1), plus a whitespace char as token
 * delimiter.
 */
#define MAX_TOKEN_NUM 1000
#define MAX_CONCURRENT_JOBS 20	//! Max number of concurrent jobs as per requirements


/**
 * @brief Struct to organize the tokens of a shell command.
 *
 * The raw input string should be saved to `cmd_str`. The tokenized command
 * should be saved to `cmd_tok`, and the size of that array to `cmd_tok_size`.
 *
 * The requirements only require to have a single pipe, which allows for a
 * maximum of 2 commands. The command to the left of the pipe symbol (or if
 * there is no pipe) should be saved as a tokenized array in `cmd1`. The command
 * to the right of the pipe should be saved to `cmd2`. If there is a pipe,
 * `pipe` should be `1`. Else, it should be `0`.
 *
 * If the command is to be run in the background, `bg` should be set to `1`, or
 * `0` for foreground.
 *
 * If there is redirection, the path to the redirection files is saved on the
 * `in`, `out` and `err` attributes. If any of those struct members is `"\0"`,
 * their default files are stdin, stdout or stderr for `in`, `out` and `err`
 * respectively.
 *
 * If there is an error parsing the command, `parsing_err` must be set to the
 * error message string. Else, `parsing_err` must be set to `"\0"`.
 */
struct Cmd {
	char cmd_str[MAX_CMD_LEN];			// Input command as a string
	char* cmd_tok[MAX_TOKEN_NUM];		// Tokenized input command
	int cmd_tok_size;					// Number of tokens in command
	char* cmd1[MAX_TOKEN_NUM];			// Command and arguments to execute
	char* cmd2[MAX_TOKEN_NUM];			// Second command if there is a pipe
	int pipe;							// Pipe boolean
	int bg;								// Background process boolean
	char in[MAX_TOKEN_LEN];				// Input redirection
	char out[MAX_TOKEN_LEN];			// Output redirection
	char err[MAX_TOKEN_LEN];			// Error redirection
	char parsing_err[MAX_ERROR_LEN];	// Parsing error message
};


void parseCmd(char* cmd_str, struct Cmd* cmd_struct);
void tokenizeString(struct Cmd* cmd_tok);

#endif

