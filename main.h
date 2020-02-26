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
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
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
#define MAX_STATUS_LEN 8	//! Max status string length
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
#define EMPTY_ARRAY -1

#define CMD_BG "bg\0"		//! Shell command bg, @sa bg()
#define CMD_FG "fg\0"		//! Shell command fg, @sa fg()
#define CMD_JOBS "jobs\0"	//! Shell command jobs, @sa jobs()

#define JOB_STATUS_RUNNING "Running\0"	//! Shell job status running
#define JOB_STATUS_STOPPED "Stopped\0"	//! Shell job status stopped
#define JOB_STATUS_DONE "Done\0"		//! Shell job status done

#define EXIT_OK 0		//! No error
#define EXIT_ERR 1		//! Unknown error
#define EXIT_ERR_ARG 2	//! Wrong argument provided
#define EXIT_ERR_CMD 3	//! Command syntax error


/**
 * @brief Struct to organize all information of a shell command.
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
struct Job {
	char cmd_str[MAX_CMD_LEN+1];		// Input command as a string
	char* cmd_tok[MAX_TOKEN_NUM];		// Tokenized input command
	uint32_t cmd_tok_len;				// Number of tokens in command
	char* cmd1[MAX_TOKEN_NUM];			// Command and arguments to execute
	char in1[MAX_TOKEN_LEN+1];			// Cmd1 input redirection
	char out1[MAX_TOKEN_LEN+1];			// Cmd1 output redirection
	char err1[MAX_TOKEN_LEN+1];			// Cmd1 error redirection
	char* cmd2[MAX_TOKEN_NUM];			// Second command if there is a pipe
	char in2[MAX_TOKEN_LEN+1];			// Cmd2 input redirection
	char out2[MAX_TOKEN_LEN+1];			// Cmd2 output redirection
	char err2[MAX_TOKEN_LEN+1];			// Cmd2 error redirection
	bool pipe;							// Pipe boolean
	bool bg;							// Background process boolean
	pid_t gpid;							// Group PID
	uint8_t jobno;						// Job number
	char status[MAX_STATUS_LEN];		// Status of the process group
	char err_msg[MAX_ERROR_LEN];		// Error message
};


// Globals
static uint8_t verbose;							//! Verbose output flag
static struct Job job_arr[MAX_CONCURRENT_JOBS];	//! Current jobs array
static int last_job = EMPTY_ARRAY;				//! Last job index in job_arr


// Functions
void initShell();
bool ignoreInput(char* input_str);
void removeJob(int job_idx);
void printJob(int job_idx);
void bgExec();
void fgExec();
void jobsExec();
bool runShellComd(char* input);
void tokenizeString(struct Job* cmd_tok);
void parseJob(char* cmd_str, struct Job jobs_arr[], int* last_job);
void redirectSimple(struct Job* cmd);
void redirectPipe(struct Job* cmd);
void waitForChildren(struct Job* cmd);
void runJob(struct Job jobs_arr[], int* last_job);
void handleNewJob(char* input);
void maintainJobsTable();
void killAllJobs();
int main(int argc, char** argv);

#endif

