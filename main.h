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
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

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
#define CHILD_COUNT_SIMPLE 1	//! Number of children processes in a simple command without pipes
#define CHILD_COUNT_PIPE 2		//! Number of children processes in a command with a pipe
#define SYSCALL_RETURN_ERR -1	//! Value returned on a system call error

#define EMPTY_STR "\0"
#define TRUE 1
#define FALSE 0

#define EXIT_OK 0
#define EXIT_ERR 1


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
 * If there is redirection, the path to the redirection files is saved on the
 * `in1`, `in2`, `out1`, `out2`, `err1` and `err2` attributes. If any of those
 * struct members is `"\0"`, they are assumed to use their default files stdin,
 * stdout or stderr.
  *
 * If the command is to be run in the background, `bg` should be set to `1`, or
 * `0` for foreground.
 *
 * If there is an error parsing or setting any part of the command, `err_msg`
 * must be set to the error message string. Else, `err_msg` must be set to
 * `"\0"`.
 */
struct Cmd {
	char cmd_str[MAX_CMD_LEN];			// Input command as a string
	char* cmd_tok[MAX_TOKEN_NUM];		// Tokenized input command
	uint32_t cmd_tok_len;				// Number of tokens in command
	char* cmd1[MAX_TOKEN_NUM];			// Command and arguments to execute
	char in1[MAX_TOKEN_LEN];			// Cmd1 input redirection
	char out1[MAX_TOKEN_LEN];			// Cmd1 output redirection
	char err1[MAX_TOKEN_LEN];			// Cmd1 error redirection
	char* cmd2[MAX_TOKEN_NUM];			// Second command if there is a pipe
	char in2[MAX_TOKEN_LEN];			// Cmd2 input redirection
	char out2[MAX_TOKEN_LEN];			// Cmd2 output redirection
	char err2[MAX_TOKEN_LEN];			// Cmd2 error redirection
	uint8_t pipe;						// Pipe boolean
	uint8_t bg;							// Background process boolean
	char err_msg[MAX_ERROR_LEN];		// Error message
};


static uint8_t ignoreInput(char* input_str);
static void tokenizeString(struct Cmd* cmd_tok);
static void parseCmd(char* cmd_str, struct Cmd* cmd);
static void redirectSimple(struct Cmd* cmd);
static void redirectPipe(struct Cmd* cmd);
static void sigint(int signo);
static void sigtstp(int signo);
static void execCmd(struct Cmd* cmd);
int main(int argc, char **argv);

#endif

